#include "Game.hpp"
#include "AudioSystem.hpp"
#include "Image.hpp"

namespace tp
{
Game::Game(AudioSystem* audioSystem)
    : audioSytem_{ audioSystem }
    , spritesBuffer_{ std::make_unique<SpritesBuffer>() }
{
    Texture groundTexture{ "images/ground.png" };
    Texture hayforksTexture{ "images/hayforks.png" };
    Texture potatoAliveTexture{ "images/potato_alive.png" };
    Texture potatoDeadEyeTexture{ "images/potato_dead_eye.png" };

    Texture combinedTexture = Texture::combineTextures(groundTexture, potatoAliveTexture);
    combinedTexture         = Texture::combineTextures(combinedTexture, potatoDeadEyeTexture);
    combinedTexture         = Texture::combineTextures(combinedTexture, hayforksTexture);

    fullImage_ = combinedTexture.image;

    auto initHayforkSprite = [this, &combinedTexture](const Vector2D position, int index) {
        spritesBuffer_->hayforks[index].init(
            position, { 0.2f, 1.0f }, combinedTexture.UVs[tp::Textures::HAYFORKS[index]]);
    };

    Vector2D hayforkPosition{ 0.5, -0.5 };
    Vector2D shift{ 0.3, 0.0 };

    for (int index = 0; index < HAYFORKS_COUNT; ++index)
        initHayforkSprite(hayforkPosition += shift, index);

    spritesBuffer_->background.init({ 0.0f, -0.9f }, { 2.0f, 0.2f }, combinedTexture.UVs["ground"]);
    spritesBuffer_->potato.init(
        { 0.0f, 0.0f }, { 0.2f, 0.15f }, combinedTexture.UVs[tp::Textures::POTATO_STILL]);

    potatoPosition_ = spritesBuffer_->potato;

    potatoGoingUpUVs_[0] = combinedTexture.UVs[Textures::POTATO_ALIVE_UP1];
    potatoGoingUpUVs_[1] = combinedTexture.UVs[Textures::POTATO_ALIVE_UP2];

    potatoGoingDownUVs_[0] = combinedTexture.UVs[Textures::POTATO_ALIVE_DOWN1];
    potatoGoingDownUVs_[1] = combinedTexture.UVs[Textures::POTATO_ALIVE_DOWN2];

    potatoDeadUVs_ = combinedTexture.UVs[Textures::POTATO_DEAD_EYE1];
}

void Game::updateGame(float deltaTime, bool isTap)
{
    if (isTap && gameGlobalData_.gameState == EGameState::StartMenu)
    {
        gameGlobalData_.gameState = EGameState::Running;
    }

    if (EGameState::Running != gameGlobalData_.gameState &&
        EGameState::Dead != gameGlobalData_.gameState)
        return;

    if (isTap)
    {
        if (EGameState::Dead == gameGlobalData_.gameState)
            gameGlobalData_.gameState = EGameState::Running;

        audioSytem_->playClickSound();
    }

    potatoMovingAnimationUpdate(deltaTime);

    potatoMovement(deltaTime, isTap);

    moveHayforks(deltaTime);
}

Image* Game::fullImage()
{
    return &fullImage_;
}

SpritesBuffer& Game::renderBuffer()
{
    return *spritesBuffer_.get();
}

void Game::potatoMovingAnimationUpdate(float deltaTime)
{
    if (EGameState::Running != gameGlobalData_.gameState)
        return;

    static int animationID = 0;

    if (frameChangeElapsed_ > 0.2f)
    {
        potatoPosition_.updateUVs(potatoGoingUpUVs_[animationID]);
        animationID         = (animationID == 0) ? 1 : 0;
        frameChangeElapsed_ = 0.0;
    }

    frameChangeElapsed_ += deltaTime;
}

void Game::potatoMovement(float deltaTime, bool isTap)
{
    const float FLOOR_LEVEL = -0.8;

    // potato jumping
    if (isTap)
    {
        potatoYVelocity_ = 0.05f;
        potatoPosition_.shift({ 0.0f, potatoYVelocity_ });
    }
    else if (spritesBuffer_->potato.center().y > FLOOR_LEVEL)
    {
        potatoYVelocity_ += -9.1f * deltaTime * deltaTime;
        potatoPosition_.shift({ 0.0f, potatoYVelocity_ });
    }
    else if (EGameState::Dead != gameGlobalData_.gameState)
    {
        gameGlobalData_.gameState = EGameState::Dead;

        // play death sound
        audioSytem_->playHitGroundSound();

        // update animation
        potatoPosition_.updateUVs(potatoDeadUVs_);
    }

    spritesBuffer_->potato = potatoPosition_;

    // potato rotation
    const Vector2D potatoCenter = spritesBuffer_->potato.center();
    const float    angle        = atan2f(0.1f, -potatoYVelocity_) - M_PI_2;

    for (auto& v : spritesBuffer_->potato.vertices)
    {
        v.coordinates.rotate(potatoCenter, angle);
    }
}

void Game::moveHayforks(float deltaTime)
{
    for (auto& hayfork : spritesBuffer_->hayforks)
    {
        if (hayfork.vertices[0].coordinates.x < -1.0f)
        {
            hayfork.shift({ 2.5f, 0.0f });
        }

        hayfork.shift({ -0.3f * deltaTime, 0.0f });
    }
}

Game::~Game() = default;
} // namespace tp

#include "Game.hpp"
#include "AudioSystem.hpp"

#include <iostream>
#include <random>

namespace tp
{

const float    VERTICAL_TAP_VELOCITY     = 0.03f;
const float    HAYFORKS_HORIZONTAL_SPEED = -0.4f;
const float    HAYFORKS_SPEED_INCREASE   = 0.1f;
const Vector2D HAYFORKS_INITIAL_POSITION = { .x = 0.5f, .y = -0.7f };
const Vector2D HARFORKS_SHIFT            = { .x = 0.3f, .y = 0.0f };

Game::Game(AudioSystem* audioSystem)
    : audioSystem_{ audioSystem }
    , spritesBuffer_{ std::make_unique<SpritesBuffer>() }
    , potatoYVelocity_{ VERTICAL_TAP_VELOCITY }
    , hayForksSpeed_{ HAYFORKS_HORIZONTAL_SPEED }
{
    Texture groundTexture{ "images/ground.png" };
    Texture hayforksTexture{ "images/hayforks.png" };
    Texture potatoAliveTexture{ "images/potato_alive.png" };
    Texture potatoDeadEyeTexture{ "images/potato_dead_eye.png" };

    Texture combinedTexture = Texture::combineTextures(groundTexture, potatoAliveTexture);
    combinedTexture         = Texture::combineTextures(combinedTexture, potatoDeadEyeTexture);
    combinedTexture         = Texture::combineTextures(combinedTexture, hayforksTexture);

    fullImage_ = combinedTexture.image;

    std::uniform_real_distribution dis(-0.3f, 0.1f);
    std::mt19937                   rgen(1u);

    auto initHayforkSprite = [this, &combinedTexture, &dis, &rgen](Vector2D position, int index) {
        static int isDown = 1;
        position.y += dis(rgen);
        position.y *= isDown;
        auto uvs = combinedTexture.UVs[Textures::HAYFORKS[index]];

        if (-1 == isDown)
        {
            flipUVsAroundX(uvs);
            position.y += 0.1f;
        }

        spritesBuffer_->hayforks[index].init(position, { 0.2f, 1.0f }, uvs);

        isDown *= -1;
    };

    Vector2D hayforksPosition{ HAYFORKS_INITIAL_POSITION };
    for (int index = 0; index < HAYFORKS_COUNT; ++index)
        initHayforkSprite(hayforksPosition += HARFORKS_SHIFT, index);

    spritesBuffer_->background.init(
        { 0.0f, -0.9f }, { 2.0f, 0.2f }, combinedTexture.UVs[Textures::GROUND]);
    spritesBuffer_->potato.init(
        { 0.0f, 0.0f }, { 0.2f, 0.15f }, combinedTexture.UVs[Textures::POTATO_STILL]);

    potatoPosition_ = spritesBuffer_->potato;

    potatoGoingUpUVs_[0] = combinedTexture.UVs[Textures::POTATO_ALIVE_UP1];
    potatoGoingUpUVs_[1] = combinedTexture.UVs[Textures::POTATO_ALIVE_UP2];

    potatoGoingDownUVs_[0] = combinedTexture.UVs[Textures::POTATO_ALIVE_DOWN1];
    potatoGoingDownUVs_[1] = combinedTexture.UVs[Textures::POTATO_ALIVE_DOWN2];

    potatoDeadUVs_ = combinedTexture.UVs[Textures::POTATO_DEAD_EYE1];
}

void Game::update(float deltaTime, bool isTap)
{
    switch (gameState_)
    {
        case EGameState::StartMenu:
        {
            if (isTap)
            {
                gameState_ = EGameState::Running;
            }
            break;
        }
        case EGameState::Running:
        {
            if (isTap)
                audioSystem_->playClickSound();

            animatePotato(deltaTime);
            movePotato(deltaTime, isTap);
            moveHayforks(deltaTime);
            checkCollisions();

            break;
        }
        case EGameState::Dead:
        case EGameState::Paused:
        case EGameState::Dying:
            break;
    }
}

Image* Game::fullImage()
{
    return &fullImage_;
}

SpritesBuffer& Game::renderBuffer()
{
    return *spritesBuffer_;
}

void Game::animatePotato(float deltaTime)
{
    if (EGameState::Running != gameState_)
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

void Game::movePotato(float deltaTime, bool isTap)
{
    const float FLOOR_LEVEL = -0.8f;

    // potato jumping
    if (isTap)
    {
        potatoYVelocity_ = VERTICAL_TAP_VELOCITY;
        potatoPosition_.shift({ 0.0f, potatoYVelocity_ });
    }
    else if (spritesBuffer_->potato.center().y > FLOOR_LEVEL)
    {
        potatoYVelocity_ += -9.1f * deltaTime * deltaTime;
        potatoPosition_.shift({ 0.0f, potatoYVelocity_ });
    }
    else if (EGameState::Dead != gameState_)
    {
        die();
    }

    spritesBuffer_->potato = potatoPosition_;

    // potato rotation
    const Vector2D potatoCenter = spritesBuffer_->potato.center();
    const float    angle        = atan2f(0.1f, -potatoYVelocity_) - static_cast<float>(M_PI_2);

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

        hayfork.shift({ hayForksSpeed_ * deltaTime, 0.0f });
    }

    static int speedIncreaseCounter = 0;
    if (spritesBuffer_->hayforks[closestHayforkIndex_].center().x < -0.1)
    {
        speedIncreaseCounter++;
        if (speedIncreaseCounter >= 10)
        {
            hayForksSpeed_ -= HAYFORKS_SPEED_INCREASE;
            speedIncreaseCounter = 0;
        }

        score_++;

        ++closestHayforkIndex_;
        if (closestHayforkIndex_ >= HAYFORKS_COUNT)
            closestHayforkIndex_ = 0;
    }
}
void Game::checkCollisions()
{
    const auto& hayfork = spritesBuffer_->hayforks[closestHayforkIndex_];
    const auto& potato  = spritesBuffer_->potato;

    if (hayfork.center().x < 0.1)
    {
        if ((hayfork.top() > 0.0f && potato.center().y > hayfork.bottom()) ||
            (hayfork.top() < 0.0 && potato.center().y < hayfork.top()))
        {
            die();
        }
    }
}

void Game::die()
{
    gameState_ = EGameState::Dead;

    // play death sound
    audioSystem_->playHitGroundSound();

    // update animation
    potatoPosition_.updateUVs(potatoDeadUVs_);

    spritesBuffer_->potato = potatoPosition_;
}

Game::~Game() = default;
} // namespace tp

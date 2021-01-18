#include "Engine.hpp"

#include "AudioSystem.hpp"
#include "EventSystem.hpp"
#include "Image.hpp"
#include "Log.hpp"
#include "Timer.hpp"
#include "VideoSystem.hpp"

#include <cmath>
#include <sstream>

namespace tp
{

Engine::Engine()
    : audio_(std::make_unique<AudioSystem>())
    , video_(std::make_unique<VideoSystem>())
{
    game_ = std::make_unique<SpritesBuffer>();

    Texture groundTexture{ "images/ground.png" };
    Texture hayforksTexture{ "images/hayforks.png" };
    Texture potatoAliveTexture{ "images/potato_alive.png" };
    Texture potatoDeadEyeTexture{ "images/potato_dead_eye.png" };

    Texture combinedTexture = Texture::combineTextures(groundTexture, potatoAliveTexture);
    combinedTexture         = Texture::combineTextures(combinedTexture, potatoDeadEyeTexture);
    combinedTexture         = Texture::combineTextures(combinedTexture, hayforksTexture);

    auto initHayforkSprite = [this, &combinedTexture](const Vector2D position, int index) {
        game_->hayforks[index].init(
            position, { 0.2f, 1.0f }, combinedTexture.UVs[tp::Textures::HAYFORKS[index]]);
    };

    Vector2D hayforkPosition{ 0.5, -0.5 };
    Vector2D shift{ 0.3, 0.0 };

    for (int index = 0; index < HAYFORKS_COUNT; ++index)
        initHayforkSprite(hayforkPosition += shift, index);

    game_->background.init({ 0.0f, -0.9f }, { 2.0f, 0.2f }, combinedTexture.UVs["ground"]);
    game_->potato.init(
        { 0.0f, 0.0f }, { 0.2f, 0.15f }, combinedTexture.UVs[tp::Textures::POTATO_STILL]);

    potatoPosition_ = game_->potato;

    video_->init(&gameInputData_.screenHorizontalScaling, *game_, combinedTexture.image);

    potatoGoingUpUVs_[0] = combinedTexture.UVs[Textures::POTATO_ALIVE_UP1];
    potatoGoingUpUVs_[1] = combinedTexture.UVs[Textures::POTATO_ALIVE_UP2];

    potatoGoingDownUVs_[0] = combinedTexture.UVs[Textures::POTATO_ALIVE_DOWN1];
    potatoGoingDownUVs_[1] = combinedTexture.UVs[Textures::POTATO_ALIVE_DOWN2];

    potatoDeadUVs_ = combinedTexture.UVs[Textures::POTATO_DEAD_EYE1];
}

void Engine::run()
{
    Timer t{};
    int   frameCount = 0;

    float deltaTime{};
    Timer frameTimer{};

    std::stringstream ss{};

    while (gameInputData_.isRunning)
    {
        gameInputData_.reset();

        frameTimer.reset();
        ++frameCount;

        if (t.elapsed() >= 1.0f)
        {
            ss << "fps: " << frameCount << ", delta time: " << deltaTime
               << ", time: " << frameTimer.initialElapsed();

            logInfo(ss.str());
            ss.str("");

            frameCount = 0;
            t.reset();
        }

        events_->pollEvents(&gameInputData_);

        if (gameInputData_.isTap && gameGlobalData_.gameState == EGameState::StartMenu)
        {
            gameGlobalData_.gameState = EGameState::Running;
        }

        updateGame(deltaTime, gameInputData_.isTap);

        video_->render(*game_, gameInputData_.screenHorizontalScaling);
        deltaTime = frameTimer.elapsed();
    }
}

void Engine::updateGame(float deltaTime, bool isTap)
{
    if (EGameState::Running != gameGlobalData_.gameState &&
        EGameState::Dead != gameGlobalData_.gameState)
        return;

    if (isTap)
    {
        if (EGameState::Dead == gameGlobalData_.gameState)
            gameGlobalData_.gameState = EGameState::Running;

        audio_->playClickSound();
    }

    potatoMovingAnimationUpdate(deltaTime);

    potatoMovement(deltaTime, isTap);

    moveHayforks(deltaTime);
}

void Engine::potatoMovingAnimationUpdate(float deltaTime)
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

void Engine::potatoMovement(float deltaTime, bool isTap)
{
    const float FLOOR_LEVEL = -0.8;

    // potato jumping
    if (isTap)
    {
        potatoYVelocity_ = 0.05f;
        potatoPosition_.shift({ 0.0f, potatoYVelocity_ });
    }
    else if (game_->potato.center().y > FLOOR_LEVEL)
    {
        potatoYVelocity_ += -9.1f * deltaTime * deltaTime;
        potatoPosition_.shift({ 0.0f, potatoYVelocity_ });
    }
    else if (EGameState::Dead != gameGlobalData_.gameState)
    {
        gameGlobalData_.gameState = EGameState::Dead;

        // play death sound
        audio_->playHitGroundSound();

        // update animation
        potatoPosition_.updateUVs(potatoDeadUVs_);
    }

    game_->potato = potatoPosition_;

    // potato rotation
    const Vector2D potatoCenter = game_->potato.center();
    const float    angle        = atan2f(0.1f, -potatoYVelocity_) - M_PI_2;

    for (auto& v : game_->potato.vertices)
    {
        v.coordinates.rotate(potatoCenter, angle);
    }
}

void Engine::moveHayforks(float deltaTime)
{
    for (auto& hayfork : game_->hayforks)
    {
        if (hayfork.vertices[0].coordinates.x < -1.0f)
        {
            hayfork.shift({ 2.5f, 0.0f });
        }

        hayfork.shift({ -0.3f * deltaTime, 0.0f });
    }
}

Engine::~Engine() = default;

} // namespace tp
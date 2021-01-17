#include "Engine.hpp"

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
    : video_(std::make_unique<VideoSystem>())
{
    game_ = std::make_unique<SpritesBuffer>();

    Texture groundTexture{ "images/ground.png" };
    Texture hayforksTexture{ "images/hayforks.png" };
    Texture potatoTexture{ "images/potato_alive.png" };

    Texture combinedTexture = Texture::combineTextures(groundTexture, potatoTexture);
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

    video_->init(gameGlobalData_, *game_, combinedTexture.image);

    potatoGoingUpUVs_[0] = combinedTexture.UVs[tp::Textures::POTATO_ALIVE_UP1];
    potatoGoingUpUVs_[1] = combinedTexture.UVs[tp::Textures::POTATO_ALIVE_UP2];

    potatoGoingDownUVs_[0] = combinedTexture.UVs[tp::Textures::POTATO_ALIVE_DOWN1];
    potatoGoingDownUVs_[1] = combinedTexture.UVs[tp::Textures::POTATO_ALIVE_DOWN2];
}

void Engine::run()
{
    Timer t{};
    int   frameCount = 0;

    float deltaTime{};
    Timer frameTimer{};

    std::stringstream ss{};

    while (gameGlobalData_.isRunning)
    {
        gameGlobalData_.reset();
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

        events_->pollEvents(gameGlobalData_);

        if (gameGlobalData_.isTap && gameGlobalData_.gameState == EGameState::StartMenu)
        {
            gameGlobalData_.gameState = EGameState::Running;
        }

        updateGame(deltaTime, gameGlobalData_.isTap);

        video_->render(*game_, gameGlobalData_.screenHorizontalScaling);
        deltaTime = frameTimer.elapsed();
    }
}

void Engine::updateGame(float deltaTime, bool isTap)
{
    if (EGameState::Running != gameGlobalData_.gameState)
        return;

    potatoAnimationUpdate(deltaTime);

    potatoMovement(deltaTime, isTap);

    moveHayforks(deltaTime);
}

void Engine::potatoAnimationUpdate(float deltaTime)
{
    static int animationID = 0;

    if (frameChangeElapsed_ > 0.2f)
    {
        potatoPosition_.updateUVs(potatoGoingUpUVs_[animationID]);

        animationID = (animationID == 0) ? 1 : 0;

        frameChangeElapsed_ = 0.0;
    }

    frameChangeElapsed_ += deltaTime;
}

void Engine::potatoMovement(float deltaTime, bool isTap)
{
    // potato jumping
    if (isTap)
    {
        potatoYVelocity_ = 0.05f;
        potatoPosition_.shift({ 0.0f, potatoYVelocity_ });
    }
    else if (game_->potato.center().y > -0.8)
    {
        potatoYVelocity_ += -9.1f * deltaTime * deltaTime;
        potatoPosition_.shift({ 0.0f, potatoYVelocity_ });
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
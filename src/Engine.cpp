#include "Engine.hpp"

#include "Constants.hpp"
#include "EventSystem.hpp"
#include "Game.hpp"
#include "Log.hpp"
#include "Timer.hpp"
#include "VideoSystem.hpp"

#include <sstream>

namespace tp
{

Engine::Engine()
    : video_(std::make_unique<VideoSystem>())
{
    game_ = std::make_unique<SpritesBuffer>();

    Texture hayforksTexture{ "images/hayforks.png" };
    Texture potatoTexture{ "images/potato_alive.png" };

    Texture combinedTexture = Texture::combineTextures(potatoTexture, hayforksTexture);

    auto initHayforkSprite = [this, &combinedTexture](const Vector2D position, int index) {
        game_->hayforks[index].init(
            position, { 0.2f, 1.0f }, combinedTexture.UVs[tp::Textures::HAYFORKS[index]]);
    };

    Vector2D hayforkPosition{ 0.5, -0.5 };
    Vector2D shift{ 0.3, 0.0 };

    for (int index = 0; index < HAYFORKS_COUNT; ++index)
        initHayforkSprite(hayforkPosition += shift, index);

    game_->potato.init(
        { 0.0f, 0.0f }, { 0.2f, 0.2f }, combinedTexture.UVs[tp::Textures::POTATO_ALIVE1]);

    video_->init(*game_, combinedTexture.image);
}

void Engine::run()
{
    bool isRunning = true;

    Timer t{};
    int   frameCount = 0;

    float deltaTime{};
    Timer frameTimer{};

    std::stringstream ss{};

    while (isRunning)
    {
        frameTimer.reset();
        ++frameCount;
        bool isTap = false;

        if (t.elapsed() >= 1.0f)
        {
            ss << "fps: " << frameCount << ", delta time: " << deltaTime
               << ", time: " << frameTimer.initialElapsed();

            logInfo(ss.str());
            ss.str("");

            frameCount = 0;
            t.reset();
        }

        processEvents(isRunning, isTap);

        updateGame(deltaTime, isTap);

        video_->render(*game_);
        deltaTime = frameTimer.elapsed();
    }
}

void Engine::processEvents(bool& isRunning, bool& isTap)
{
    EventType eventType;
    if (events_->pollEvents(eventType))
    {
        switch (eventType)
        {
            case EventType::Quit:
            {
                isRunning = false;
                break;
            }
            case EventType::Click:
            {
                logInfo("Mouse Click");
                isTap = true;
                break;
            }
        }
    }
}

void Engine::updateGame(float deltaTime, bool isTap)
{
    // potato jumping
    if (isTap)
    {
        potatoYVelocity_ = 0.05f;
        game_->potato.shift({ 0.0f, potatoYVelocity_ });
    }
    else if (game_->potato.vertices[0].coordinates.y >= -0.8)
    {
        potatoYVelocity_ += -9.1f * deltaTime * deltaTime;
        game_->potato.shift({ 0.0f, potatoYVelocity_ });
    }

    // hay forks movement
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
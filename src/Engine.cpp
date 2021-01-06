#include "Engine.hpp"

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
    Image potatoAliveImage("images/potato_alive.png");
    game_ = std::make_unique<SpritesBuffer>();

    Vector2D potatoUVs[4] = {
        0.2f, 1.0f, // top right
        0.2f, 0.0f, // bottom right
        0.0f, 0.0f, // bottom left
        0.0f, 1.0f, // top left
    };

    game_->potato.init({ 0.0f, 0.0f }, { 0.2f, 0.2f }, potatoUVs);
    game_->hayforks[0].init({ 0.0f, 0.0f }, { 0.2f, 0.2f }, potatoUVs);

    video_->init(*game_, potatoAliveImage);
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

        // video_->render(deltaTime, frameTimer.initialElapsed(), isTap);
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
    auto hayfork = &game_->hayforks[0];

    if (hayfork->vertices[0].coordinates.x < -1.0f)
    {
        hayfork->shift({ 2.0f, 0.0f });
    }

    hayfork->shift({-0.3f * deltaTime, 0.0f});
}

Engine::~Engine() = default;

} // namespace tp

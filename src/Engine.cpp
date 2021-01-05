#include "Engine.hpp"

#include "EventSystem.hpp"
#include "Log.hpp"
#include "Timer.hpp"
#include "VideoSystem.hpp"

#include <iostream>

namespace tp
{

Engine::Engine()
    : video_(std::make_unique<VideoSystem>())
{
    video_->init();
}

void Engine::run()
{
    bool isRunning = true;

    Timer t{};
    int   frameCount = 0;

    float deltaTime{};
    Timer frameTimer{};

    while (isRunning)
    {
        frameTimer.reset();
        ++frameCount;

        if (t.elapsed() >= 1.0f)
        {
            std::cout << "fps: " << frameCount << ", delta time: " << deltaTime
                      << ", time: " << frameTimer.initialElapsed() << std::endl;
            frameCount = 0;
            t.reset();
        }
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
                    break;
                }
            }
        }

        video_->render(deltaTime, frameTimer.initialElapsed());
        deltaTime = frameTimer.elapsed();
    }
}

Engine::~Engine() = default;

} // namespace tp

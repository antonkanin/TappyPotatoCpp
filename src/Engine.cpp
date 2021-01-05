#include "Engine.hpp"

#include "EventSystem.hpp"
#include "Log.hpp"
#include "Timer.hpp"
#include "VideoSystem.hpp"

#include <sstream>

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

        video_->render(deltaTime, frameTimer.initialElapsed(), isTap);
        deltaTime = frameTimer.elapsed();
    }
}

Engine::~Engine() = default;

} // namespace tp

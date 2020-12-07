#include "Engine.hpp"

#include <iostream>

#include "EventSystem.hpp"
#include "VideoSystem.hpp"

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
    while (isRunning)
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
                    std::cout << "Mouse Click" << std::endl;
                }
            }
        }

        video_->render();
    }
}

Engine::~Engine() = default;

} // namespace tp

#include "Engine.hpp"

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
    while (true)
    {
        events_->processEvents(isRunning);
        if (!isRunning)
            break;

        video_->render();
    }
}

Engine::~Engine() = default;

} // namespace tp

#include "Engine.hpp"

#include "AudioSystem.hpp"
#include "EventSystem.hpp"
#include "Game.hpp"
#include "Log.hpp"
#include "Timer.hpp"
#include "VideoSystem.hpp"

#include <sstream>

namespace tp
{

Engine::Engine()
    : audio_(std::make_unique<AudioSystem>())
    , video_(std::make_unique<VideoSystem>())
    , game_(std::make_unique<Game>(audio_.get()))

{
    video_->init(&inputData_.screenHorizontalScaling, game_->renderBuffer(), *game_->fullImage());
}

void Engine::run()
{
    Timer t{};
    int   frameCount = 0;

    float deltaTime{};
    Timer frameTimer{};

    std::stringstream ss{};

    while (inputData_.isRunning)
    {
        inputData_.reset();

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

        events_->pollEvents(&inputData_);

        game_->update(deltaTime, inputData_.isTap);

        video_->render(game_->renderBuffer(), inputData_.screenHorizontalScaling, game_->score_);

        deltaTime = frameTimer.elapsed();
    }
}

Engine::~Engine() = default;

} // namespace tp
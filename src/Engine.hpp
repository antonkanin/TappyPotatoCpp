#pragma once

#include "Game.hpp"
#include <memory>

namespace tp
{
class VideoSystem;
class EventSystem;

class Engine final
{
public:
    Engine();
    ~Engine();

    void run();

private:
    void updateGame(float deltaTime, bool isTap);

    std::unique_ptr<VideoSystem> video_{};
    std::unique_ptr<EventSystem> events_{};

    std::unique_ptr<struct SpritesBuffer> game_{};

    GameGlobalState gameGlobalState_{};

    float potatoYVelocity_{};
};
} // namespace tp
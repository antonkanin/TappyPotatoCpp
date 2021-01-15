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

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&)                 = delete;
    Engine& operator=(Engine&&) = delete;

    void run();

private:
    void updateGame(float deltaTime, bool isTap);

    std::unique_ptr<VideoSystem> video_{};
    std::unique_ptr<EventSystem> events_{};

    std::unique_ptr<struct SpritesBuffer> game_{};

    GameGlobalData gameGlobalState_{};

    float  potatoYVelocity_{};
    Sprite potatoPosition_{};

    PotatoAnimationState potatoAnimationState_{ PotatoAnimationState::Stationary };
};
} // namespace tp
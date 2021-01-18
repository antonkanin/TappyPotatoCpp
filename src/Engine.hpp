#pragma once

#include "Constants.hpp"
#include "Game.hpp"

#include <memory>

namespace tp
{
class AudioSystem;
class EventSystem;
class VideoSystem;

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
    void potatoAnimationUpdate(float deltaTime);
    void potatoMovement(float deltaTime, bool isTap);
    void moveHayforks(float deltaTime);

    std::unique_ptr<AudioSystem> audio_{};
    std::unique_ptr<EventSystem> events_{};
    std::unique_ptr<VideoSystem> video_{};

    std::unique_ptr<struct SpritesBuffer> game_{};

    GameGlobalData gameGlobalData_{};

    float  potatoYVelocity_{};
    Sprite potatoPosition_{};

    float   frameChangeElapsed_{ 0.0f };
    FourUVs potatoGoingUpUVs_[POTATO_GOING_UP_FRAMES]{};
    FourUVs potatoGoingDownUVs_[POTATO_GOING_DOWN_FRAMES]{};
};
} // namespace tp
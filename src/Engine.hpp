#pragma once

#include <memory>

namespace tp
{
class AudioSystem;
class EventSystem;
class VideoSystem;
class Game;

struct InputData
{
    bool  isRunning{ true };
    bool  isTap{ false };
    float screenHorizontalScaling{ 0.0f };
    float tapX{};
    float tapY{};

    void reset() { isTap = false; }
};

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
    std::unique_ptr<AudioSystem> audio_{};
    std::unique_ptr<EventSystem> events_{};
    std::unique_ptr<VideoSystem> video_{};

    std::unique_ptr<Game> game_{};

    InputData inputData_;
};
} // namespace tp
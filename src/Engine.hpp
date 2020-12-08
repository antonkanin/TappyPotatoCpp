#pragma once

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
    std::unique_ptr<VideoSystem> video_{};
    std::unique_ptr<EventSystem> events_{};
};
} // namespace tp
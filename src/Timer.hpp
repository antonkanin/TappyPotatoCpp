#pragma once

#include <chrono>

class Timer final
{
public:
    Timer()
        : start_{ std::chrono::high_resolution_clock::now() }
        , initialStart_{ start_ }
    {
    }

    [[nodiscard]] float elapsed() const
    {
        const auto end = std::chrono::high_resolution_clock::now();
        return getElapsed(start_, end);
    }

    [[nodiscard]] float initialElapsed() const
    {
        const auto end = std::chrono::high_resolution_clock::now();
        return getElapsed(initialStart_, end);
    }

    void reset() { start_ = std::chrono::high_resolution_clock::now(); }

private:
    using TimePoint = std::chrono::high_resolution_clock::time_point;

    static float getElapsed(TimePoint start, TimePoint end)
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
               (1000.0f * 1000.0f);
    }

    TimePoint start_{};
    TimePoint initialStart_{};
};

#pragma once

#include <chrono>

class Timer
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
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count() /
               1000.0f;
    }

    [[nodiscard]] float initialElapsed() const
    {
        const auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - initialStart_).count() /
               1000.0f;
    }

    void reset() { start_ = std::chrono::high_resolution_clock::now(); }

private:
    std::chrono::high_resolution_clock::time_point start_{};
    std::chrono::high_resolution_clock::time_point initialStart_{};
};

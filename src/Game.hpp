#pragma once

#include <algorithm>
#include <cstring>

#include "Constants.hpp"
#include "Math.hpp"
#include "Sprite.hpp"

namespace tp
{

struct SpritesBuffer
{
    std::array<Sprite, HAYFORKS_COUNT> hayforks{};
    Sprite                             background{};
    Sprite                             potato{};
};

struct SpritesRawBuffer
{
    std::array<Sprite, sizeof(SpritesBuffer) / sizeof(Sprite)> sprites;
};

static_assert(sizeof(SpritesBuffer) == sizeof(SpritesRawBuffer));

constexpr int SPRITES_COUNT = sizeof(SpritesBuffer) / sizeof(Sprite);

enum class EGameState
{
    StartMenu,
    Running,
    Paused,
    Dying,
    Dead
};

struct GameInputData
{
    bool  isRunning{ true };
    bool  isTap{ false };
    float screenHorizontalScaling{ 0.0f };
    float tapX{};
    float tapY{};

    void reset() { isTap = false; }
};

struct GameGlobalData
{
    EGameState gameState{ EGameState::StartMenu };
    int score{ 0 };
};

} // namespace tp
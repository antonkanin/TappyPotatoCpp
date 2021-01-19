#pragma once

#include <algorithm>
#include <cstring>
#include <memory>

#include "Constants.hpp"
#include "Image.hpp"
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

struct GameGlobalData
{
    EGameState gameState{ EGameState::StartMenu };
    int        score{ 0 };
};

class AudioSystem;

class Game final
{
public:
    Game(AudioSystem* audioSystem);
    ~Game();

    Game(Game&)   = delete;
    Game& operator=(Game&) = delete;
    Game(Game&&)           = delete;
    Game& operator=(Game&&) = delete;

    void update(float deltaTime, bool isTap);

    Image* fullImage();

    SpritesBuffer& renderBuffer();

    int score_{ 0 };

private:
    void animatePotato(float deltaTime);
    void movePotato(float deltaTime, bool isTap);
    void moveHayforks(float deltaTime);
    void checkCollisions();
    void die();

    int closestHayforkIndex_{ 0 };

    AudioSystem* audioSystem_{}; // TODO(Anton) it would be nice if game didn't know anything about
                                 // the audiosystem

    std::unique_ptr<struct SpritesBuffer> spritesBuffer_{};

    using HayforkCollider = std::array<Vector2D, 3>;
    std::array<HayforkCollider, HAYFORKS_COUNT> hayforkColliders_{};

    float  potatoYVelocity_{};
    Sprite potatoPosition_{};

    float   frameChangeElapsed_{ 0.0f };
    FourUVs potatoGoingUpUVs_[POTATO_GOING_UP_FRAMES]{};
    FourUVs potatoGoingDownUVs_[POTATO_GOING_DOWN_FRAMES]{};
    FourUVs potatoDeadUVs_{};

    EGameState gameState_{ EGameState::StartMenu };
    Image      fullImage_{};
};

} // namespace tp
#include "Engine.hpp"

#include "EventSystem.hpp"
#include "Image.hpp"
#include "Log.hpp"
#include "Timer.hpp"
#include "VideoSystem.hpp"

#include <cmath>
#include <sstream>

namespace tp
{

Engine::Engine()
    : video_(std::make_unique<VideoSystem>())
{
    game_ = std::make_unique<SpritesBuffer>();

    Texture hayforksTexture{ "images/hayforks.png" };
    Texture potatoTexture{ "images/potato_alive.png" };

    Texture combinedTexture = Texture::combineTextures(potatoTexture, hayforksTexture);

    auto initHayforkSprite = [this, &combinedTexture](const Vector2D position, int index) {
        ;
        game_->hayforks[index].init(
            position, { 0.2f, 1.0f }, combinedTexture.UVs[tp::Textures::HAYFORKS[index]]);
    };

    Vector2D hayforkPosition{ 0.5, -0.5 };
    Vector2D shift{ 0.3, 0.0 };

    for (int index = 0; index < HAYFORKS_COUNT; ++index)
        initHayforkSprite(hayforkPosition += shift, index);

    game_->potato.init(
        { 0.0f, 0.0f }, { 0.4f, 0.3f }, combinedTexture.UVs[tp::Textures::POTATO_ALIVE1]);

    potatoPosition_ = game_->potato;

    video_->init(gameGlobalState_, *game_, combinedTexture.image);
}

void Engine::run()
{
    Timer t{};
    int   frameCount = 0;

    float deltaTime{};
    Timer frameTimer{};

    std::stringstream ss{};

    while (gameGlobalState_.isRunning)
    {
        gameGlobalState_.reset();
        frameTimer.reset();
        ++frameCount;

        if (t.elapsed() >= 1.0f)
        {
            // ss << "fps: " << frameCount << ", delta time: " << deltaTime
            //    << ", time: " << frameTimer.initialElapsed();
            //
            // logInfo(ss.str());
            // ss.str("");

            frameCount = 0;
            t.reset();
        }

        events_->pollEvents(gameGlobalState_);

        updateGame(deltaTime, gameGlobalState_.isTap);

        video_->render(*game_, gameGlobalState_);
        deltaTime = frameTimer.elapsed();
    }
}

void Engine::updateGame(float deltaTime, bool isTap)
{
    // potato jumping
    if (isTap)
    {
        potatoYVelocity_ = 0.05f;
        potatoPosition_.shift({ 0.0f, potatoYVelocity_ });
    }
    else if (game_->potato.center().y > -1.0)
    {
        potatoYVelocity_ += -9.1f * deltaTime * deltaTime;
        potatoPosition_.shift({ 0.0f, potatoYVelocity_ });
    }

    game_->potato = potatoPosition_;

    // potato rotation
    const Vector2D potatoCenter = game_->potato.center();
    const float    angle        = atan2f(0.1f, -potatoYVelocity_) - M_PI_2;

    for (auto& v : game_->potato.vertices)
    {
        v.coordinates.rotate(potatoCenter, angle);
    }

    // hay forks movement
    for (auto& hayfork : game_->hayforks)
    {
        if (hayfork.vertices[0].coordinates.x < -1.0f)
        {
            hayfork.shift({ 2.5f, 0.0f });
        }

        hayfork.shift({ -0.3f * deltaTime, 0.0f });
    }
}

Engine::~Engine() = default;

} // namespace tp
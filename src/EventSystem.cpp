#include "EventSystem.hpp"

#include <cassert>

#include <glad/glad.h>

#ifdef __ANDROID__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "Exceptions.hpp"
#include "Game.hpp"

namespace tp
{

EventSystem::EventSystem()
{
    if (!SDL_InitSubSystem(SDL_INIT_EVENTS))
        throw Exception("Could not initialized SDL Events " + std::string(SDL_GetError()));
}

void EventSystem::pollEvents(InputData* gameInputData)
{
    assert(gameInputData != nullptr);

    SDL_Event sdlEvent{};

    while (SDL_PollEvent(&sdlEvent))
    {
        switch (sdlEvent.type)
        {
            case SDL_QUIT:
            {
                gameInputData->isRunning = false;
                break;
            }
            case SDL_WINDOWEVENT:
            {
                if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    auto screeWidth   = sdlEvent.window.data1;
                    auto screenHeight = sdlEvent.window.data2;
                    gameInputData->screenHorizontalScaling =
                        static_cast<float>(screenHeight) / static_cast<float>(screeWidth);
                    glViewport(0, 0, screeWidth, screenHeight);
                }

                // TODO process windows events
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                gameInputData->isTap = true;
                break;
            }
            case SDL_KEYDOWN:
            {
                switch (sdlEvent.key.keysym.sym)
                {
                    case SDLK_SPACE:
                    {
                        gameInputData->isTap = true;
                        break;
                    }
                    case SDLK_ESCAPE:
                    {
                        gameInputData->isRunning = false;
                        break;
                    }
                }
                break;
            }
        }
    }
}

} // namespace tp
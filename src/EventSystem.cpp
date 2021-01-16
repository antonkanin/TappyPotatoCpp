#include "EventSystem.hpp"

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

bool EventSystem::pollEvents(GameGlobalData& gameGlobalData)
{
    SDL_Event sdlEvent{};

    while (SDL_PollEvent(&sdlEvent))
    {
        switch (sdlEvent.type)
        {
            case SDL_QUIT:
            {
                gameGlobalData.isRunning = false;
                return true;
            }
            case SDL_WINDOWEVENT:
            {
                if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    auto screeWidth   = sdlEvent.window.data1;
                    auto screenHeight = sdlEvent.window.data2;
                    gameGlobalData.screenHorizontalScaling =
                        static_cast<float>(screenHeight) / static_cast<float>(screeWidth);
                    glViewport(0, 0, screeWidth, screenHeight);
                }

                // TODO process windows events
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                // TODO(Anton): we should probably move this logic to the game
                // and only return the fact that user clicked
                if (EGameState::StartMenu == gameGlobalData.gameState)
                {
                    gameGlobalData.gameState = EGameState::Running;
                }

                gameGlobalData.isTap = true;
                return true;
            }
        }
    }

    return false;
}

} // namespace tp
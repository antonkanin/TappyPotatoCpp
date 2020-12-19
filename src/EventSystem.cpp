#include "EventSystem.hpp"

#include <glad/glad.h>

#ifdef __ANDROID__
#include <SDL.h>
#include <SDL_events.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#endif

#include "Exceptions.hpp"

namespace tp
{

EventSystem::EventSystem()
{
    if (!SDL_InitSubSystem(SDL_INIT_EVENTS))
        throw Exception("Could not initialized SDL Events " + std::string(SDL_GetError()));
}

bool EventSystem::pollEvents(EventType& eventType)
{
    SDL_Event sdlEvent{};

    while (SDL_PollEvent(&sdlEvent))
    {
        switch (sdlEvent.type)
        {
            case SDL_QUIT:
            {
                eventType = EventType::Quit;
                return true;
            }
            case SDL_WINDOWEVENT:
            {
                if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    auto screeWidth   = sdlEvent.window.data1;
                    auto screenHeight = sdlEvent.window.data2;
                    glViewport(0, 0, screeWidth, screenHeight);
                }

                // TODO process windows events
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                eventType = EventType::Click;
                return true;
            }
        }
    }

    return false;
}

} // namespace tp
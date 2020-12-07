#include "EventSystem.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

#include "Exceptions.hpp"

namespace tp
{

EventSystem::EventSystem()
{
    if (!SDL_InitSubSystem(SDL_INIT_EVENTS))
        throw Exception("Could not initialized SDL Events " + std::string(SDL_GetError()));
}

void EventSystem::processEvents(bool& isActive)
{
    SDL_Event event{};

    while (SDL_PollEvent(&event))
    {
        if (SDL_QUIT == event.type)
            isActive = false;

        // if (SDL_WINDOWEVENT == event.type)
        // {
        //     if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        //     {
        //         auto width  = event.window.data1;
        //         auto height = event.window.data2;
        //         glViewport(0, 0, width, height);
        //     }
        // }
    }
}

} // namespace tp
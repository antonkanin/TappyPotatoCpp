#pragma once

#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <string>

#include "Exceptions.hpp"

namespace tp
{
class VideoSystem final
{
public:
    void init() noexcept(false);
    void render();

private:
    SDL_GLContext context_{};
    SDL_Window*   window_{};
};

} // namespace tp
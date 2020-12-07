#include "VideoSystem.hpp"

#ifdef _WIN32
#include <windows.h> // required to compile gl.h with VS compiler
#endif

#include <iostream>
#include <string>

#include <GL/gl.h>
#include <SDL2/SDL.h>

#include "Exceptions.hpp"

namespace tp
{

// FIXME(Anton) move these to pimpl
SDL_GLContext context_{};
SDL_Window*   window_{};

const char* vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "uniform vec4 ourColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   // "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "   FragColor = ourColor;\n"
                                   "}\n\0";

void VideoSystem::init() noexcept(false)
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
        throw Exception("Could not initialize SDL2 Video System " + std::string(SDL_GetError()));

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    const int   SCREEN_WIDTH  = 800;
    const int   SCREEN_HEIGHT = 600;
    const char* SCREEN_TITLE  = "Tappy Potato";

    window_ = SDL_CreateWindow(SCREEN_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);

    if (!window_)
        throw Exception("Could not create window: " + std::string(SDL_GetError()));

    context_ = SDL_GL_CreateContext(window_);

    // typedef
    // initializeGLFunction();

    // void* func_pointer = SDL_GL_GetProcAddress("glCreateShader");

    // typedef unsigned int GLenum;

    // typedef void(glFuncPtr_t)(GLenum cap);
    // auto glEnable = reinterpret_cast<glFuncPtr_t*>(func_pointer);

    // if (glEnable == nullptr)
    //     throw Exception("Could not initialize glCreateShader");

    glEnable(GL_DEPTH_TEST);
    std::cout << "Error: " << glGetError() << std::endl;

    // SDL_LoadFunction()
}

void VideoSystem::render()
{
    SDL_Delay(2000);
}

} // namespace tp
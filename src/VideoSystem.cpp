#include "VideoSystem.hpp"

#ifdef _WIN32
#include <windows.h> // required to compile gl.h with VS compiler
#endif

#include <string>

#ifdef __ANDROID__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <glad/glad.h>

#include "Exceptions.hpp"
#include "Log.hpp"
#include "TextRenderer.hpp"
#include "Utils.hpp"

namespace tp
{

const char* vertexShaderSource = "#version 300 es\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";

const char* fragmentShaderSource = "#version 300 es\n"
                                   "out mediump vec4 FragColor;\n"
                                   // "uniform vec4 ourColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   // "   FragColor = ourColor;\n"
                                   "}\n\0";

class VideoSystemImpl final
{
public:
    SDL_GLContext context_{};
    SDL_Window*   window_{};

    ShaderProgram rectShader_{};
    ShaderProgram textShader_{};

    // GLuint_t shaderProgram_{};
    GLuint_t VAO_{};

    TextRenderer textRenderer_{};
};

VideoSystem::VideoSystem()
    : pi{ std::make_unique<VideoSystemImpl>() }
{
}

void VideoSystem::init() noexcept(false)
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
        throw Exception("Could not initialize SDL2 Video System " + std::string(SDL_GetError()));

    if (!SDL_SetHint(SDL_HINT_ORIENTATIONS, "Portrait"))
        logInfo("Could not set orientation to Portrait");

    const int   SCREEN_WIDTH  = 800;
    const int   SCREEN_HEIGHT = 600;
    const char* SCREEN_TITLE  = "Tappy Potato";

    pi->window_ = SDL_CreateWindow(SCREEN_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if (!pi->window_)
        throw Exception("Could not create window: " + std::string(SDL_GetError()));

    if (0 != SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG))
        throw Exception("Could not set "
                        "SDL_GL_CONTEXT_FLAGS: " +
                        std::string(SDL_GetError()));

    if (0 != SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3))
        throw Exception(
            "Could not set SDL_GL_CONTEXT_MAJOR_VERSION: " + std::string(SDL_GetError()));

    if (0 != SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0))
        throw Exception(
            "Could not set SDL_GL_CONTEXT_MINOR_VERSION: " + std::string(SDL_GetError()));

    // int profileType = SDL_GL_CONTEXT_PROFILE_CORE;
    int profileType = SDL_GL_CONTEXT_PROFILE_ES;
    if (0 != SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profileType))
        throw Exception(
            "Could not set SDL_GL_CONTEXT_PROFILE_MASK: " + std::string(SDL_GetError()));

    //// Check OpenGL version
    int glMajorVersion{};
    int glMinorVersion{};

    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &glMajorVersion);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &glMinorVersion);

    logInfo(
        "OpenGL version " + std::to_string(glMajorVersion) + "." + std::to_string(glMinorVersion));

    pi->context_ = SDL_GL_CreateContext(pi->window_);
    if (!pi->context_)
        throw Exception("Could not create OpenGl context: " + std::string(SDL_GetError()));

    if (0 == gladLoadGLES2Loader(SDL_GL_GetProcAddress))
        throw Exception("Could not initialize glad");

    // glEnable(GL_DEPTH_TEST);
    // GL_CHECK()

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //// Initialize GL data
    initializeVAO();

    pi->rectShader_.init(vertexShaderSource, fragmentShaderSource);

    pi->textRenderer_.init();
}

void VideoSystem::render()
{
    // pi->rectShader_.use();
    //
    // glBindVertexArray(pi->VAO_);
    // GL_CHECK()
    //
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    // GL_CHECK()
    //
    // glBindVertexArray(0);
    // GL_CHECK()

    pi->textRenderer_.renderText("Test", 0.0f, 0.0f, 0.01f);

    // SDL_Delay(2000);

    SDL_GL_SwapWindow(pi->window_);
}

void VideoSystem::initializeVAO()
{
    // clang-format off
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // bearingY right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom bearingX
        -0.5f,  0.5f, 0.0f   // bearingY bearingX
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };
    // clang-format on

    glGenVertexArrays(1, &pi->VAO_);
    GL_CHECK()

    glBindVertexArray(pi->VAO_);
    GL_CHECK()

    GLuint VBO{};
    glGenBuffers(1, &VBO);
    GL_CHECK()

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    GL_CHECK()

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    GL_CHECK()

    GLuint EBO{};
    glGenBuffers(1, &EBO);
    GL_CHECK()

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    GL_CHECK()

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    GL_CHECK()

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
    GL_CHECK()

    glEnableVertexAttribArray(0);
    GL_CHECK()
}

VideoSystem::~VideoSystem()
{
    SDL_Quit();
}

} // namespace tp
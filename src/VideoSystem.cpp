#include "VideoSystem.hpp"

#ifdef _WIN32
#include <windows.h> // required to compile gl.h with VS compiler
#endif

#include <iostream>
#include <string>
#include <cassert>

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include "Exceptions.hpp"

namespace tp
{

// FIXME(Anton) move these to pimpl
SDL_GLContext context_{};
SDL_Window*   window_{};

#define GL_CHECK()                                                                                 \
    {                                                                                              \
        const int err = static_cast<int>(glGetError());                                            \
        if (err != GL_NO_ERROR)                                                                    \
        {                                                                                          \
            switch (err)                                                                           \
            {                                                                                      \
                case GL_INVALID_ENUM:                                                              \
                    std::cerr << "GL_INVALID_ENUM" << std::endl;                                   \
                    break;                                                                         \
                case GL_INVALID_VALUE:                                                             \
                    std::cerr << "GL_INVALID_VALUE" << std::endl;                                  \
                    break;                                                                         \
                case GL_INVALID_OPERATION:                                                         \
                    std::cerr << "GL_INVALID_OPERATION" << std::endl;                              \
                    break;                                                                         \
                case GL_INVALID_FRAMEBUFFER_OPERATION:                                             \
                    std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;                  \
                    break;                                                                         \
                case GL_OUT_OF_MEMORY:                                                             \
                    std::cerr << "GL_OUT_OF_MEMORY" << std::endl;                                  \
                    break;                                                                         \
            }                                                                                      \
            assert(false);                                                                         \
        }                                                                                          \
    }

const char* vertexShaderSource = "#version 300 es\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";

const char* fragmentShaderSource = "#version 300 es\n"
                                   "out lowp vec4 FragColor;\n"
                                   // "uniform vec4 ourColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   // "   FragColor = ourColor;\n"
                                   "}\n\0";

void VideoSystem::init() noexcept(false)
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
        throw Exception("Could not initialize SDL2 Video System " + std::string(SDL_GetError()));

    const int   SCREEN_WIDTH  = 800;
    const int   SCREEN_HEIGHT = 600;
    const char* SCREEN_TITLE  = "Tappy Potato";

    window_ = SDL_CreateWindow(SCREEN_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if (!window_)
        throw Exception("Could not create window: " + std::string(SDL_GetError()));

    if (0 != SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG))
        throw Exception("Could not set "
                        "SDL_GL_CONTEXT_FLAGS: " +
                        std::string(SDL_GetError()));

    if (0 != SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3))
        throw Exception(
            "Could not set SDL_GL_CONTEXT_MAJOR_VERSION: " + std::string(SDL_GetError()));

    if (0 != SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2))
        throw Exception(
            "Could not set SDL_GL_CONTEXT_MINOR_VERSION: " + std::string(SDL_GetError()));

    if (0 != SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES))
        throw Exception(
            "Could not set SDL_GL_CONTEXT_PROFILE_MASK: " + std::string(SDL_GetError()));
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    context_ = SDL_GL_CreateContext(window_);
    if (!context_)
        throw Exception("Could not create OpenGl context: " + std::string(SDL_GetError()));

    //// Check OpenGL version
    int glMajorVersion{};
    int glMinorVersion{};

    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &glMajorVersion);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &glMinorVersion);

    std::cout << "OpenGL version " << glMajorVersion << '.' << glMinorVersion << std::endl;

    if (0 == gladLoadGLES2Loader(SDL_GL_GetProcAddress))
        throw Exception("Could not initialize glad");

    glEnable(GL_DEPTH_TEST);

    //// Initialize GL data
    initializeVAO();

    shaderProgram_ = glCreateProgram();
    linkShaderProgram(shaderProgram_);
}

GLuint VideoSystem::createShader(const char*& shaderSourceCode, int shaderType) const
{
    GLuint shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderSourceCode, nullptr);
    glCompileShader(shaderId);

    int success{};
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        const int logSize = 512;
        char      infoLog[logSize];
        glGetShaderInfoLog(shaderId, logSize, nullptr, infoLog);
        throw Exception("Shader error: " + std::string(infoLog));
    }

    return shaderId;
}
void VideoSystem::linkShaderProgram(GLuint_t shaderProgram_) noexcept(false)
{
    auto vertexShader   = createShader(vertexShaderSource, GL_VERTEX_SHADER);
    auto fragmentShader = createShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    glAttachShader(shaderProgram_, vertexShader);
    glAttachShader(shaderProgram_, fragmentShader);
    glLinkProgram(shaderProgram_);

    {
        int success{};
        glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
        if (!success)
        {
            const int logSize = 512;
            char      infoLog[logSize];
            glGetProgramInfoLog(shaderProgram_, logSize, nullptr, infoLog);
            throw Exception("Shader linking error: " + std::string(infoLog));
        }
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void VideoSystem::render()
{
    glUseProgram(shaderProgram_);
    GL_CHECK()

    glBindVertexArray(VAO_);
    GL_CHECK()

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    GL_CHECK()

    glBindVertexArray(0);
    GL_CHECK()

    // SDL_Delay(2000);

    SDL_GL_SwapWindow(window_);
}

void VideoSystem::initializeVAO()
{
    // clang-format off
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };
    // clang-format on

    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);

    GLuint VBO{};
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint EBO{};
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
}

VideoSystem::~VideoSystem()
{
    SDL_Quit();
}

} // namespace tp
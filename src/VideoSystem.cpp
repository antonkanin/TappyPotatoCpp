#include "VideoSystem.hpp"

#ifdef _WIN32
#include <windows.h> // required to compile gl.h with VS compiler
#endif

#include <cassert>
#include <string>

#ifdef __ANDROID__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <glad/glad.h>

#include "Exceptions.hpp"
#include "Game.hpp"
#include "Log.hpp"
#include "TextRenderer.hpp"
#include "Utils.hpp"

namespace tp
{

const char* vertexShaderSource = "#version 300 es\n"
                                 "// potato vertex shader\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "layout (location = 1) in vec2 aTexCoord;\n"
                                 "out mediump vec2 texCoord;"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "   texCoord = aTexCoord;"
                                 "}\0";

const char* fragmentShaderSource = "#version 300 es\n"
                                   "// potato fragment shader\n"
                                   "out mediump vec4 FragColor;\n"
                                   "in mediump vec2 texCoord;"
                                   // "uniform vec4 ourColor;\n"
                                   "uniform sampler2D textureSampler;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = texture(textureSampler, texCoord);\n"
                                   // "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   // "   FragColor = ourColor;\n"
                                   "}\n\0";

class VideoSystemImpl final
{
public:
    SDL_GLContext context_{};
    SDL_Window*   window_{};

    ShaderProgram spriteShader_{};

    GLuint_t VAO_{};
    GLuint   VBO_{};
    GLuint   textureId_{};

    TextRenderer textRenderer_{};
};

VideoSystem::VideoSystem()
    : pi{ std::make_unique<VideoSystemImpl>() }
{
}

void VideoSystem::init(const SpritesBuffer& buffer, const Image& texture) noexcept(false)
{
    createWindowAndGlContext();

    initializeVertexBuffer(buffer);

    initializeTexture(texture);

    pi->spriteShader_.init(vertexShaderSource, fragmentShaderSource);

    pi->textRenderer_.init();
}

void VideoSystem::render(const SpritesBuffer& buffer)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, pi->textureId_);

    glBindVertexArray(pi->VAO_);
    GL_CHECK()

    glBindBuffer(GL_ARRAY_BUFFER, pi->VBO_);
    GL_CHECK()

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(buffer), &buffer);
    GL_CHECK()

    pi->spriteShader_.use();
    glDrawElements(GL_TRIANGLES, 6 * sizeof(buffer) / sizeof(Sprite), GL_UNSIGNED_INT, nullptr);
    GL_CHECK()

    glBindVertexArray(0);
    GL_CHECK()

    // pi->textRenderer_.renderText("Test", 0.0f, 0.0f, 0.005f);

    SDL_GL_SwapWindow(pi->window_);
}

void VideoSystem::createWindowAndGlContext()
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
        throw Exception("Could not initialize SDL2 Video System " + std::string(SDL_GetError()));

    if (!SDL_SetHint(SDL_HINT_ORIENTATIONS, "Portrait"))
        logInfo("Could not set orientation to Portrait");

    const int   SCREEN_WIDTH  = 800;
    const int   SCREEN_HEIGHT = 600;
    const char* SCREEN_TITLE  = "Tappy Potato";

    pi->window_ = SDL_CreateWindow(SCREEN_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if (!pi->window_)
        throw Exception("Could not create window: " + std::string(SDL_GetError()));

#ifdef TP_DEBUG
    SDL_SetWindowPosition(pi->window_, 100, 100);
#endif

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

    // SDL_GL_SetSwapInterval(0); // disable VSync

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void VideoSystem::initializeVertexBuffer(const SpritesBuffer& buffer)
{
    glGenVertexArrays(1, &pi->VAO_);
    GL_CHECK()

    glBindVertexArray(pi->VAO_);
    GL_CHECK()

    {
        glGenBuffers(1, &pi->VBO_);
        GL_CHECK()

        glBindBuffer(GL_ARRAY_BUFFER, pi->VBO_);
        GL_CHECK()

        glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), &buffer, GL_STATIC_DRAW);
        GL_CHECK()
    }

    {
        unsigned int indices[] = {
            // clang-format off
            0, 1, 3,  // first triangle
            1, 2, 3,  // second triangle

            4, 5, 7,  // first triangle
            5, 6, 7   // second triangle
            // clang-format on
        };

        GLuint EBO{};
        glGenBuffers(1, &EBO);
        GL_CHECK()

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        GL_CHECK()

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
        GL_CHECK()
    }

    // vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)nullptr);
    GL_CHECK()
    glEnableVertexAttribArray(0);
    GL_CHECK()

    // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3D)));
    GL_CHECK()
    glEnableVertexAttribArray(1);
    GL_CHECK()
}

void VideoSystem::initializeTexture(const Image& image)
{
    assert(image.data() != nullptr);

    glGenTextures(1, &pi->textureId_);
    glBindTexture(GL_TEXTURE_2D, pi->textureId_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA,
        GL_UNSIGNED_BYTE, image.data());
}

VideoSystem::~VideoSystem()
{
    SDL_Quit();
}

} // namespace tp
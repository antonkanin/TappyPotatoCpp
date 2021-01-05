#include "VideoSystem.hpp"

#ifdef _WIN32
#include <windows.h> // required to compile gl.h with VS compiler
#endif

#include <array>
#include <cassert>
#include <iostream>
#include <string>

#ifdef __ANDROID__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <glad/glad.h>

#include "Exceptions.hpp"
#include "Image.hpp"
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

struct Sprite
{
    float vertices[12];
    float uv[8];
};

struct GameSprites
{
    Sprite potato;
    Sprite hayforks[10];
    Sprite background;
};

class VideoSystemImpl final
{
public:
    SDL_GLContext context_{};
    SDL_Window*   window_{};

    ShaderProgram rectShader_{};

    // GLuint_t shaderProgram_{};
    GLuint_t VAO_{};
    GLuint   VBO_{};
    GLuint   potatoTexture_{};

    TextRenderer                 textRenderer_{};
    std::unique_ptr<GameSprites> gameSprites_{};

    // 24 verticies + 16 UVs = Total 40
    std::array<float, 40> vertices_{};

    float velocityY_{};
};

VideoSystem::VideoSystem()
    : pi{ std::make_unique<VideoSystemImpl>() }
{
}

void VideoSystem::init() noexcept(false)
{
    initializeWindowAndContext();

    initializeGameSprites();
    //// Initialize GL data
    initializeVAO();
    // initializeVAO2();

    // loadTexture();

    pi->rectShader_.init(vertexShaderSource, fragmentShaderSource);

    pi->textRenderer_.init();
}

void shiftCoordinate(float* firstElement, float shift)
{
    for (int index = 0; index < 4; ++index)
    {
        *firstElement += shift;
        firstElement += 3;
    }
}

void VideoSystem::render(float deltaTime, float time, bool isTap)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    static float timeElapsed{};
    timeElapsed += deltaTime;
    if (timeElapsed > 0.5)
    {
        std::cout << "pos: " << pi->vertices_[1] << ", vel: " << pi->velocityY_ << std::endl;
        timeElapsed = 0.0f;
    }

    // potato jumping
    if (isTap)
        pi->velocityY_ = 0.05f;
    else
        pi->velocityY_ += -9.1f * deltaTime * deltaTime;

    shiftCoordinate(&pi->vertices_[1], pi->velocityY_);

    // moving potato
    float* hayForkPositionPtr = &pi->vertices_[12];
    if (*hayForkPositionPtr < -1.0)
    {
        *hayForkPositionPtr       = 1.4;
        *(hayForkPositionPtr + 3) = 1.4;
        *(hayForkPositionPtr + 6) = 1.0;
        *(hayForkPositionPtr + 9) = 1.0;
    }

    shiftCoordinate(hayForkPositionPtr, -0.3f * deltaTime);

    pi->rectShader_.use();

    glBindTexture(GL_TEXTURE_2D, pi->potatoTexture_);

    glBindVertexArray(pi->VAO_);
    GL_CHECK()

    glBindBuffer(GL_ARRAY_BUFFER, pi->VBO_);
    GL_CHECK()

    glBufferSubData(GL_ARRAY_BUFFER, 0, pi->vertices_.size() * sizeof(float), pi->vertices_.data());
    GL_CHECK()

    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
    GL_CHECK()

    glBindVertexArray(0);
    GL_CHECK()

    // pi->textRenderer_.renderText("Test", 0.0f, 0.0f, 0.005f);

    // SDL_Delay(2000);

    SDL_GL_SwapWindow(pi->window_);
}

void VideoSystem::initializeWindowAndContext()
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

    SDL_GL_SetSwapInterval(0); // disable VSync
    //     SDL_GL_SetSwapInterval(1); // disable VSync

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void VideoSystem::initializeGameSprites()
{
    pi->gameSprites_ = std::make_unique<GameSprites>();

    pi->gameSprites_->potato = {

        // clang-format off
        // position          // UV
        0.5f,  0.5f, 0.0f,   0.2f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,   0.2f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left
    };
    // clang-format on

    // TODO(Anton) come up with the sprite atlas
    /*
     *
     ALIVE_01 tr br bl tl
     ALIVE_02
     ALIVE_03

     * */

    // load sprites
    Image potatoAliveImage("images/potato_alive.png");

    glGenTextures(1, &pi->potatoTexture_);
    glBindTexture(GL_TEXTURE_2D, pi->potatoTexture_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    assert(potatoAliveImage.data() != nullptr);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, potatoAliveImage.width(), potatoAliveImage.height(), 0,
        GL_RGBA, GL_UNSIGNED_BYTE, potatoAliveImage.data());

    // merge sprites into one buffer

    // load buffer into GPU
}

void VideoSystem::initializeVAO()
{
    pi->vertices_ = {
        // clang-format off
        // Vertices for sprite #1
         0.2f,  0.2f, 0.0f, // top right
         0.2f, -0.2f, 0.0f, // bottom right
        -0.2f, -0.2f, 0.0f, // bottom left
        -0.2f,  0.2f, 0.0f, // top left

        // Vertices for sprite #2
         0.2f,  1.0f, 0.0f, // top right
         0.2f,  0.6f, 0.0f, // bottom right
        -0.2f,  0.6f, 0.0f, // bottom left
        -0.2f,  1.0f, 0.0f, // top left

        // UVs for sprite #1
        0.2f, 1.0f,   // top right
        0.2f, 0.0f,   // bottom right
        0.0f, 0.0f,   // bottom left
        0.0f, 1.0f,   // top left

        // UVs for sprite #2
        0.2f, 1.0f,   // top right
        0.2f, 0.0f,   // bottom right
        0.0f, 0.0f,   // bottom left
        0.0f, 1.0f    // top left
        // clang-format on
    };

    unsigned int indices[] = {
        // clang-format off
        0, 1, 3,  // first triangle
        1, 2, 3,  // second triangle

        4, 5, 7,  // first triangle
        5, 6, 7   // second triangle
        // clang-format on
    };

    glGenVertexArrays(1, &pi->VAO_);
    GL_CHECK()

    glBindVertexArray(pi->VAO_);
    GL_CHECK()

    {
        glGenBuffers(1, &pi->VBO_);
        GL_CHECK()

        glBindBuffer(GL_ARRAY_BUFFER, pi->VBO_);
        GL_CHECK()

        glBufferData(GL_ARRAY_BUFFER, pi->vertices_.size() * sizeof(float), pi->vertices_.data(),
            GL_STATIC_DRAW);
        GL_CHECK()
    }

    {
        GLuint EBO{};
        glGenBuffers(1, &EBO);
        GL_CHECK()

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        GL_CHECK()

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
        GL_CHECK()
    }

    // vertices
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)nullptr);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
    GL_CHECK()
    glEnableVertexAttribArray(0);
    GL_CHECK()

    // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(2 * 12 * sizeof(float)));
    GL_CHECK()
    glEnableVertexAttribArray(1);
    GL_CHECK()
}

VideoSystem::~VideoSystem()
{
    SDL_Quit();
}

} // namespace tp
#include "TextRenderer.hpp"

#include "Constants.hpp"
#include "Exceptions.hpp"
#include "Log.hpp"
#include "Utils.hpp"

#include <glad/glad.h>
#include <vector>

#ifdef __ANDROID__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

namespace tp
{

const char* textVertexShader = "#version 300 es\n"
                               "layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\n"
                               "out mediump vec2 TexCoords;\n"
                               //"uniform mat4 projection;\n"
                               "void main()\n"
                               "{\n"
                               // "  gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
                               "    gl_Position = vec4(vertex.xy, 0.0, 1.0);\n"
                               "    TexCoords   = vertex.zw;\n"
                               "}";

const char* textFragmentShader =
    "#version 300 es\n"
    "in mediump vec2 TexCoords;\n"
    "out mediump vec4 color;\n"
    "uniform sampler2D text;\n"
    "uniform mediump vec3      textColor;\n"
    "void main()\n"
    "{\n"
    "    mediump vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
    "    color        = vec4(textColor, 1.0) * sampled;\n"
    "}";

void TextRenderer::initFontTextures()
{
    FT_Library freeTypeLibrary;
    if (FT_Init_FreeType(&freeTypeLibrary))
        throw Exception("Could not initialize FreeType library");

    SDL_RWops* rw = SDL_RWFromFile(Constants::FONT_FILE, "r");
    if (nullptr == rw)
        throw Exception("Could not open file: " + std::string(Constants::FONT_FILE));

    const Sint64 fileSize = SDL_RWsize(rw);

    std::vector<unsigned char> buffer(fileSize);

    Sint64 readTotal = 0;
    Sint64 read      = -1;

    while (readTotal < fileSize && read != 0)
    {
        read = SDL_RWread(rw, &buffer[readTotal], 1, fileSize - readTotal);
        readTotal += read;
    }

    FT_Face face;
    if (FT_New_Memory_Face(
            freeTypeLibrary, &buffer[0], static_cast<FT_Long>(buffer.size()), 0, &face))
        throw Exception("Failed to load font " + std::string(Constants::FONT_FILE));

    if (FT_Set_Pixel_Sizes(face, 0, 48))
        throw Exception("Failed to set font size for " + std::string(Constants::FONT_FILE));

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            logError("Failed to load Glyph");
            continue;
        }

        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        GL_CHECK();

        glBindTexture(GL_TEXTURE_2D, texture);
        GL_CHECK();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows,
            0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        GL_CHECK();

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GL_CHECK();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GL_CHECK();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GL_CHECK();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GL_CHECK();

        // now store character for later use
        Character character = { .textureID = texture,
            .sizeX                         = face->glyph->bitmap.width,
            .sizeY                         = face->glyph->bitmap.rows,
            .bearingX                      = face->glyph->bitmap_left,
            .bearingY                      = face->glyph->bitmap_top,
            .advance                       = face->glyph->advance.x };

        characters_.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(freeTypeLibrary);
}

void TextRenderer::initCharacterVertexBuffer()
{
    glGenVertexArrays(1, &VAO_);
    GL_CHECK();

    glGenBuffers(1, &VBO_);
    GL_CHECK();

    glBindVertexArray(VAO_);
    GL_CHECK();

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    GL_CHECK();

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    GL_CHECK();
    glEnableVertexAttribArray(0);
    GL_CHECK();
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    GL_CHECK();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_CHECK();

    glBindVertexArray(0);
    GL_CHECK();
}

void TextRenderer::init()
{
    initFontTextures();

    initCharacterVertexBuffer();

    shaderProgram_.init(textVertexShader, textFragmentShader);
}

void TextRenderer::renderText(std::string text, float x, float y, float scale)
{
    // activate corresponding render state
    shaderProgram_.use();

    glUniform3f(glGetUniformLocation(shaderProgram_.program(), "textColor"), 0.5, 1.0, 0.5);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO_);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        const Character ch = characters_[*c];

        float xpos = x + ch.bearingX * scale;
        float ypos = y - static_cast<float>(ch.sizeY - ch.bearingY) * scale;

        float w = ch.sizeX * scale;
        float h = ch.sizeY * scale;

        // clang-format off
        // update VBO for each character
        float vertices[6][4] = {
            { xpos, ypos + h, 0.0f, 0.0f },
            { xpos, ypos, 0.0f, 1.0f },
            { xpos + w, ypos, 1.0f, 1.0f },
            { xpos, ypos + h, 0.0f, 0.0f },
            { xpos + w, ypos, 1.0f, 1.0f },
            { xpos + w, ypos + h, 1.0f, 0.0f }
        };
        // clang-format on

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}
} // namespace tp
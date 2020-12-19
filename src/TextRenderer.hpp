#pragma once

#include <map>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H
#include "ShaderProgram.hpp"

namespace tp
{

struct Character
{
    unsigned int textureID{}; // ID handle of the glyph texture
    unsigned int sizeX{};
    unsigned int sizeY{};    // sizeY
    FT_Int       bearingX{};    // bearing X
    FT_Int       bearingY{};     // bearing Y
    FT_Int       advance{}; // Offset to advance to next glyph
};

class TextRenderer
{
public:
    void init();
    void renderText(std::string text, float x, float y, float scale);

private:
    std::map<char, Character> characters_{};
    ShaderProgram shaderProgram_{};

    unsigned int VAO_{};
    unsigned int VBO_{};
};

} // namespace tp
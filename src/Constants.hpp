#pragma once

#include <array>

namespace tp
{

namespace Constants
{
inline const char* FONT_FILE = "fonts/segoeuisl.ttf";
} // namespace Constants

const int HAYFORKS_COUNT = 8;
const int SPRITES_COUNT  = HAYFORKS_COUNT + 1; // adding one for potato

namespace Textures
{
inline const char* POTATO_ALIVE1 = "potato_alive1";
inline const char* POTATO_ALIVE2 = "potato_alive2";
inline const char* POTATO_ALIVE3 = "potato_alive3";
inline const char* POTATO_ALIVE4 = "potato_alive4";

const std::array<const char*, HAYFORKS_COUNT> HAYFORKS = {
    // clang-format off
    "hayfork1",
    "hayfork2",
    "hayfork3",
    "hayfork4",
    "hayfork5",
    "hayfork6",
    "hayfork7",
    "hayfork8"
    // clang-format off
};

} // namespace Textures

} // namespace tp

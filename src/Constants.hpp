#pragma once

#include <array>

namespace tp
{

namespace Constants
{
inline const char* FONT_FILE = "fonts/segoeuisl.ttf";
} // namespace Constants

const int HAYFORKS_COUNT           = 8;
const int POTATO_GOING_UP_FRAMES   = 2;
const int POTATO_GOING_DOWN_FRAMES = 2;

namespace Textures
{
inline const char* POTATO_STILL       = "potato_still";
inline const char* POTATO_ALIVE_UP1   = "potato_alive_up1";
inline const char* POTATO_ALIVE_UP2   = "potato_alive_up2";
inline const char* POTATO_ALIVE_DOWN1 = "potato_alive_down1";
inline const char* POTATO_ALIVE_DOWN2 = "potato_alive_down2";

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

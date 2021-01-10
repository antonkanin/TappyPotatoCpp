#pragma once

#include <unordered_map>

namespace tp
{

struct Vector2D final
{
    float x{};
    float y{};
};

struct Vector3D final
{
    float x{};
    float y{};
    float z{};

    Vector3D operator+=(const Vector2D& value)
    {
        x += value.x;
        y += value.y;
        return *this;
    }
};

struct Vertex
{
    Vector3D coordinates;
    Vector2D uv;
};

using FourUVs = std::array<Vector2D, 4>;
using UVMap   = std::unordered_map<std::string, FourUVs>;

} // namespace tp
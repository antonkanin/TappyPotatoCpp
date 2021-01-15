#pragma once

#include <cmath>
#include <string>
#include <unordered_map>

namespace tp
{

struct Vector2D final
{
    float x{};
    float y{};

    Vector2D operator+=(const Vector2D& value)
    {
        x += value.x;
        y += value.y;
        return *this;
    }
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

    Vector3D& rotate(const Vector2D& aroundPoint, float angle)
    {
        const float s = sinf(angle);
        const float c = cosf(angle);

        float newX = x - aroundPoint.x;
        float newY = y - aroundPoint.y;

        x = c * newX - s * newY;
        y = s * newX + c * newY;

        x += aroundPoint.x;
        y += aroundPoint.y;

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
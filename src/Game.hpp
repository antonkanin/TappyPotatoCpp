#pragma once

#include <cstring>

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

struct Sprite final
{
    Vertex vertices[4];

    void init(const Vector2D& center, const Vector2D& size, const Vector2D (&uvs)[4]) noexcept
    {
        // top right
        vertices[0].coordinates.x = center.x + size.x * 0.5f;
        vertices[0].coordinates.y = center.y + size.y * 0.5f;
        vertices[0].uv            = uvs[0];

        // bottom right
        vertices[1].coordinates.x = center.x + size.x * 0.5f;
        vertices[1].coordinates.y = center.y - size.y * 0.5f;
        vertices[1].uv            = uvs[1];

        // bottom left
        vertices[2].coordinates.x = center.x - size.x * 0.5f;
        vertices[2].coordinates.y = center.y - size.y * 0.5f;
        vertices[2].uv            = uvs[2];

        // top left
        vertices[3].coordinates.x = center.x - size.x * 0.5f;
        vertices[3].coordinates.y = center.y + size.y * 0.5f;
        vertices[3].uv            = uvs[3];
    }

    void shift(const Vector2D& shift) noexcept
    {
        for (auto& vertex : vertices)
        {
            vertex.coordinates += shift;
        }
    }
};

struct SpritesBuffer
{
    Sprite hayforks[1];
    Sprite potato{};
};

} // namespace tp
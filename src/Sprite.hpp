#pragma once

#include <array>

#include "Math.hpp"

namespace tp
{

struct Sprite final
{
    // Vertex vertices[4];
    std::array<Vertex, 4> vertices;

    void init(const Vector2D& center, const Vector2D& size, const FourUVs& uvs) noexcept
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

    [[nodiscard]] Vector2D center() const
    {
        // clang-format off
        return {
            vertices[2].coordinates.x + (vertices[0].coordinates.x - vertices[2].coordinates.x) * 0.5f,
            vertices[2].coordinates.y + (vertices[0].coordinates.y - vertices[2].coordinates.y) * 0.5f
        };
        // clang-format on
    }

    [[nodiscard]] float top() const
    {
        return vertices[0].coordinates.y;
    }

    [[nodiscard]] float bottom() const
    {
        return vertices[1].coordinates.y;
    }

    void updateUVs(const FourUVs& uvs)
    {
        for (int index = 0; index < vertices.size(); ++index)
        {
            vertices[index].uv = uvs[index];
        }
    }
};

} // namespace tp
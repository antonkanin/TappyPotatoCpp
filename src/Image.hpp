#pragma once

#include <string>

// #include "Game.hpp"
#include "Math.hpp"

namespace tp
{
struct Image final
{
public:
    explicit Image(const std::string& fileName) noexcept(false);

    Image() = default;
    Image(const Image& other);
    Image& operator=(Image other);

    Image(Image&& other) noexcept;

    ~Image();

    [[nodiscard]] int            width() const noexcept;
    [[nodiscard]] int            height() const noexcept;
    [[nodiscard]] unsigned char* data() const noexcept;

    friend void swap(Image& left, Image& right) noexcept
    {
        using namespace std;
        swap(left.width_, right.width_);
        swap(left.height_, right.height_);
        swap(left.channels_, right.channels_);
        swap(left.buffer_, right.buffer_);
    }

    static Image combineImages(const Image& topImage, const Image& bottomImage) noexcept(false);

private:
    int            width_{};
    int            height_{};
    int            channels_{};
    unsigned char* buffer_{};
};

struct Texture final
{
    Image image{};
    UVMap UVs{};

    explicit Texture(const std::string& fileName) noexcept(false);

    Texture(Image _image, UVMap _UVs)
        : image{ std::move(_image) }
        , UVs{ std::move(_UVs) }
    {
    }

    static Texture combineTextures(const Texture& top, const Texture& bottom);

private:
    static UVMap readUVsFromFile(const std::string& fileName);

    static UVMap combineUVs(const Texture& top, const Texture& bottom);
};

} // namespace tp
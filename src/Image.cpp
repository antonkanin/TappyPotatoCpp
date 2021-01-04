#include "Image.hpp"

#include "Exceptions.hpp"
#include "Utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image/stb_image.h>

namespace tp
{

Image::Image(const std::string& fileName) noexcept(false)
{
    stbi_set_flip_vertically_on_load(1);
    const auto rawData = getData(fileName);
    if (rawData.empty())
        throw Exception("The image file is empty: " + fileName);

    buffer_ = stbi_load_from_memory(&rawData[0], rawData.size(), &width_, &height_, &channels_, 0);

    if (!buffer_)
        throw Exception("STBI could not load image file: " + fileName);
};

Image::Image(const Image& other)
    : width_{ other.width_ }
    , height_{ other.height_ }
    , channels_{ other.channels_ }
{
    memcpy(buffer_, other.buffer_, width_ * height_ * channels_);
}

Image& Image::operator=(Image other)
{
    swap(*this, std::move(other));
    return *this;
}

Image::Image(Image&& other) noexcept
    : Image()
{
    swap(*this, other);
}

Image::~Image()
{
    delete buffer_;
}

int Image::width() const noexcept
{
    return width_;
}

int Image::height() const noexcept
{
    return height_;
}
unsigned char* Image::data() const noexcept
{
    return buffer_;
}

} // namespace tp

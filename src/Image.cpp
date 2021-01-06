#include "Image.hpp"

#include "Exceptions.hpp"
#include "Utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image/stb_image.h>

#include <cstring>
#include <string_view>

namespace tp
{

void swap(Image& left, Image right) noexcept;

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

void copyImage(unsigned char*& destination, const Image& destinationImage, const Image& fromImage)
{
    if (fromImage.width() < destinationImage.width())
    {
        for (int y = 0; y < fromImage.height(); ++y)
        {
            unsigned char* fromMemory = fromImage.data() + y * fromImage.width() * 4;
            std::memcpy(destination, fromMemory, fromImage.width() * 4);
            destination += destinationImage.width() * 4;
        }
    }
    else
    {
        std::memcpy(destination, fromImage.data(), fromImage.width() * fromImage.height() * 4);
    }
}

Image Image::combineImages(const Image& topImage, const Image& bottomImage)
{
    assert(topImage.width_ != 0 && topImage.height() != 0);
    assert(topImage.buffer_ != nullptr);
    assert(bottomImage.width_ != 0 && bottomImage.height() != 0);
    assert(bottomImage.buffer_ != nullptr);
    assert(topImage.channels_ == bottomImage.channels_);

    Image result;

    result.width_    = std::max(topImage.width(), bottomImage.width());
    result.height_   = topImage.height() + bottomImage.height();
    result.channels_ = topImage.channels_;

    const size_t imageSize = result.width() * result.height() * 4;
    result.buffer_         = new unsigned char[imageSize];

    // fill in the buffer with a nice pink color
    for (size_t index = 0; index < imageSize; index += 4)
    {
        result.buffer_[index]     = 0xF3;
        result.buffer_[index + 1] = 0x01;
        result.buffer_[index + 2] = 0x72;
        result.buffer_[index + 3] = 0xFF;
    }

    unsigned char* colorPtr = result.buffer_;

    copyImage(colorPtr, result, topImage);
    copyImage(colorPtr, result, bottomImage);

    return result;
}

} // namespace tp

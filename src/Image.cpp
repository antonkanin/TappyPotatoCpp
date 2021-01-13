#include "Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image/stb_image.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <sstream>
#include <string_view>
#include <unordered_map>

#include "Exceptions.hpp"
#include "Utils.hpp"

namespace tp
{

Image::Image(const std::string& fileName) noexcept(false)
{
    stbi_set_flip_vertically_on_load(1);
    const auto rawData = getData(fileName);
    if (rawData.empty())
        throw Exception("The image file is empty: " + fileName);

    buffer_ = stbi_load_from_memory(
        &rawData[0], static_cast<int>(rawData.size()), &width_, &height_, &channels_, 0);

    if (!buffer_)
        throw Exception("STBI could not load image file: " + fileName);
};

Image::Image(const Image& other)
    : width_{ other.width_ }
    , height_{ other.height_ }
    , channels_{ other.channels_ }
{
    const int newSize = width_ * height_ * channels_;
    buffer_           = new unsigned char[newSize];

    memcpy(buffer_, other.buffer_, newSize);
}

Image& Image::operator=(Image other)
{
    swap(*this, other);
    return *this;
}

Image::Image(Image&& other) noexcept
    : Image()
{
    swap(*this, other);
}

Image::~Image()
{
    delete[] buffer_;
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

    const int imageSize = result.width() * result.height() * 4;
    result.buffer_      = new unsigned char[imageSize];

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

void clearLine(std::string& line)
{
    ltrim(line);
    rtrim(line);

    // remove comments
    line = line.substr(0, line.find("//"));
}

std::unordered_map<std::string, FourUVs> Texture::readUVsFromFile(const std::string& fileName)
{
    auto data = getData(fileName);

    std::unordered_map<std::string, FourUVs> result{};

    auto getNextLineEnd = [&data](decltype(data)::iterator& prevLineEnd) {
        auto lineBegin = prevLineEnd;
        auto lineEnd   = std::find(prevLineEnd, data.end(), '\n');

        if (lineEnd != data.end())
            prevLineEnd = lineEnd + 1;
        else
            prevLineEnd = lineEnd;

        // making sure Windows text files are handled properly
        if (*(lineEnd - 1) == '\r')
            lineEnd--;

        return std::make_pair(lineBegin, lineEnd);
    };

    int  lineCount{};
    auto nextLineBegin = data.begin();

    while (nextLineBegin != data.end())
    {
        auto [lineBegin, lineEnd] = getNextLineEnd(nextLineBegin);
        std::string line{ lineBegin, lineEnd };

        lineEnd++;

        ++lineCount;
        clearLine(line);
        if (line.empty())
            continue;

        if (':' != line[0])
        {
            std::stringstream error{};
            error << "Error in the UV file: " << fileName
                  << ", expecting ':' as a start of the new UV id in line " +
                         std::to_string(lineCount) + ": " + line;

            throw Exception(error.str());
        }

        auto uvId = line.substr(1, line.size() - 1);

        FourUVs UVs{};

        for (auto& uv : UVs)
        {
            ++lineCount;
            const auto [coordsLineBegin, coordsLineEnd] = getNextLineEnd(nextLineBegin);
            std::string coordsLine{ coordsLineBegin, coordsLineEnd };

            std::stringstream coordsStream{ coordsLine };

            if (!(coordsStream >> uv.x >> uv.y))
            {
                std::stringstream error{};
                error << "Error in the UV file: " << fileName
                      << ", expecting two UV coordinates in line " + std::to_string(lineCount) +
                             ": " + coordsLine;
                throw Exception(error.str());
            }
        }

        result.emplace(uvId, UVs);
    }

    return result;
}

Texture::Texture(const std::string& fileName) noexcept(false)
{
    auto extPosition = fileName.find(".png");
    if (extPosition == std::string::npos)
        throw Exception("Image can be only PNG: " + fileName);

    const auto fileNameWithoutExt = fileName.substr(0, extPosition);

    image = Image{ fileName };
    UVs   = readUVsFromFile(fileNameWithoutExt + ".uv");
}

Texture Texture::combineTextures(const Texture& top, const Texture& bottom)
{
    auto combinedImage = Image::combineImages(top.image, bottom.image);

    auto combinedUVs = combineUVs(top, bottom);

    return { combinedImage, combinedUVs };
}

UVMap Texture::combineUVs(const Texture& top, const Texture& bottom)
{
    UVMap result{};
    result.reserve(top.UVs.size() + bottom.UVs.size());

    auto transformUVs = [](UVMap* result, const UVMap& sourceMap, float widthScale,
                            float heightScale, float verticalShift) {
        for (auto element : sourceMap)
        {
            for (auto& UV : element.second)
            {
                UV.x *= widthScale;
                UV.y *= heightScale;
                UV.y += verticalShift;
            }

            (*result).insert({ element.first, element.second });
        }
    };

    const float width = std::max(top.image.width(), bottom.image.width());
    const float height =
        static_cast<float>(top.image.height()) + static_cast<float>(bottom.image.height());

    transformUVs(&result, top.UVs, static_cast<float>(top.image.width()) / width,
        static_cast<float>(top.image.height()) / height, 0.0f);

    transformUVs(&result, bottom.UVs, static_cast<float>(bottom.image.width()) / width,
        static_cast<float>(bottom.image.height()) / height,
        static_cast<float>(top.image.height()) / height);

    return result;
}

} // namespace tp

#pragma once

typedef unsigned int GLuint_t;

#include "Image.hpp"
#include <map>
#include <memory>

namespace tp
{

struct SpritesBuffer;
class Image;

class VideoSystem final
{
public:
    VideoSystem();
    ~VideoSystem();

    void init(const SpritesBuffer& buffer, const Image& texture) noexcept(false);
    // void render(float deltaTime, float time, bool isTap);
    void render(const SpritesBuffer& buffer);

private:
    std::unique_ptr<class VideoSystemImpl> pi;

    void createWindowAndGlContext();
    void initializeVertexBuffer(const SpritesBuffer& buffer);
    void initializeTexture(const Image& image);
};

} // namespace tp
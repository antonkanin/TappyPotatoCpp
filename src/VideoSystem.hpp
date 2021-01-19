#pragma once


#include <memory>

namespace tp
{

struct SpritesBuffer;
struct Image;

class VideoSystem final
{
public:
    VideoSystem();
    ~VideoSystem();

    void init(float* horizontalScaling, const SpritesBuffer& buffer,
        const Image& texture) noexcept(false);
    void render(const SpritesBuffer& buffer, float screenHorizontalScaling, int score);

private:
    std::unique_ptr<class VideoSystemImpl> pi;

    float createWindowAndGlContext();
    void initializeVertexBuffer(const SpritesBuffer& buffer);
    void initializeTexture(const Image& image);
};

} // namespace tp
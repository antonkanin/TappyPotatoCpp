#pragma once

typedef unsigned int GLuint_t;

#include <map>
#include <memory>

namespace tp
{

class VideoSystem final
{
public:
    VideoSystem();
    ~VideoSystem();

    void init() noexcept(false);
    void render();

private:
    std::unique_ptr<class VideoSystemImpl> pi;

    void initializeVAO();
    void loadTexture();
};

} // namespace tp
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

    void initializeWindowAndContext();
    void initializeGameSprites();

    void initializeVAO();
};

} // namespace tp
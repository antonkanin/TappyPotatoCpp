#include "Utils.hpp"

#include "Exceptions.hpp"

#include <glad/glad.h>

#ifdef __ANDROID__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

namespace tp
{

void glCheck()
{
    const int err = static_cast<int>(glGetError());
    if (err != GL_NO_ERROR)
    {
        switch (err)
        {
            case GL_INVALID_ENUM:
                throw Exception("GL_INVALID_ENUM");
            case GL_INVALID_VALUE:
                throw Exception("GL_INVALID_VALUE");
            case GL_INVALID_OPERATION:
                throw Exception("GL_INVALID_OPERATION");
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                throw Exception("GL_INVALID_FRAMEBUFFER_OPERATION");
            case GL_OUT_OF_MEMORY:
                throw Exception("GL_OUT_OF_MEMORY");
        }
    }
}

std::vector<unsigned char> getData(const std::string& fileName) noexcept(false)
{
    SDL_RWops* rw = SDL_RWFromFile(fileName.c_str(), "r");
    if (nullptr == rw)
        throw Exception("Could not open file: " + fileName);

    const Sint64 fileSize = SDL_RWsize(rw);
    std::vector<unsigned char> result(fileSize);

    Sint64 readTotal = 0;
    Sint64 read      = -1;

    while (readTotal < fileSize && read != 0)
    {
        read = SDL_RWread(rw, &result[readTotal], 1, fileSize - readTotal);
        readTotal += read;
    }

    return result;
}

} // namespace tp
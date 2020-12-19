#include "Utils.hpp"

#include "Exceptions.hpp"

#include <glad/glad.h>

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

} // namespace tp
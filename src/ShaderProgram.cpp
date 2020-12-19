#include "ShaderProgram.hpp"

#include "Exceptions.hpp"
#include "Utils.hpp"

#include <glad/glad.h>

namespace tp
{

void ShaderProgram::init(const char* vertexShaderCode, const char* fragmentShaderCode)
{
    auto vertexShader   = createShader(vertexShaderCode, GL_VERTEX_SHADER);
    auto fragmentShader = createShader(fragmentShaderCode, GL_FRAGMENT_SHADER);

    shaderProgram_ = glCreateProgram();

    glAttachShader(shaderProgram_, vertexShader);
    GL_CHECK()

    glAttachShader(shaderProgram_, fragmentShader);
    GL_CHECK()

    glLinkProgram(shaderProgram_);
    GL_CHECK()

    {
        int success{};
        glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
        GL_CHECK()

        if (!success)
        {
            const int logSize = 512;
            char      infoLog[logSize];
            glGetProgramInfoLog(shaderProgram_, logSize, nullptr, infoLog);
            GL_CHECK()

            throw Exception("ShaderProgram linking error: " + std::string(infoLog));
        }
    }

    glDeleteShader(vertexShader);
    GL_CHECK()

    glDeleteShader(fragmentShader);
    GL_CHECK()
}

unsigned int ShaderProgram::createShader(const char*& shaderSourceCode, int shaderType)
{
    GLuint shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderSourceCode, nullptr);
    GL_CHECK()

    glCompileShader(shaderId);
    GL_CHECK()

    int success{};
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    GL_CHECK()

    if (!success)
    {
        const int logSize = 512;
        char      infoLog[logSize];

        glGetShaderInfoLog(shaderId, logSize, nullptr, infoLog);
        GL_CHECK()

        throw Exception("ShaderProgram error: " + std::string(infoLog));
    }

    return shaderId;
}

void ShaderProgram::use() const
{
    glUseProgram(shaderProgram_);
    GL_CHECK()
}

unsigned int ShaderProgram::program() const
{
    return shaderProgram_;
}

} // namespace tp

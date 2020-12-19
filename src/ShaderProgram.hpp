#pragma once

#include <string>

namespace tp
{
class ShaderProgram
{
public:
    void                       init(const char* vertexShaderCode, const char* fragmentShaderCode);
    void                       use() const;
    [[nodiscard]] unsigned int program() const;

private:
    unsigned int createShader(const char*& shaderSourceCode, int shaderType);

    unsigned int shaderProgram_;
};
} // namespace tp
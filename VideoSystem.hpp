#pragma once

typedef unsigned int GLuint_t;

namespace tp
{
class VideoSystem final
{
public:
    ~VideoSystem();
    void init() noexcept(false);
    void render();

private:
    GLuint_t shaderProgram_{};
    GLuint_t VAO_{};

    unsigned int createShader(const char*& shaderSourceCode, int shaderType) const;
    void linkShaderProgram(GLuint_t shaderProgram_) noexcept(false);

    void initializeVAO();

};

} // namespace tp
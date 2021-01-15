#pragma once

#include <string>
#include <vector>

namespace tp
{
#ifdef DEBUG_OPENGL_ON
void glCheck();

#define GL_CHECK() glCheck();
#else
#define GL_CHECK()
#endif

std::vector<unsigned char> getData(const std::string& fileName) noexcept(false);

void ltrim(std::string& line);
void rtrim(std::string& line);

} // namespace tp
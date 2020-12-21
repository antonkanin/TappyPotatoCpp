#pragma once

namespace tp
{
#ifdef DEBUG_OPENGL_ON
void glCheck();

#define GL_CHECK() glCheck();
#else
#define GL_CHECK()
#endif

} // namespace tp
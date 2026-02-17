#pragma once

#ifdef _WIN32
#    include "WGL/WGLSurface.h"
namespace OpenGL
{
    using Surface = WGLSurface;
};
#else
#    error "Only WIN32 is supported"
#endif
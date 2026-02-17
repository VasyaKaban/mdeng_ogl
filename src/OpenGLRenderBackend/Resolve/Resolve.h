#pragma once

#ifdef _WIN32
#    include "WGL/WGLResolve.h"
namespace OpenGL
{
    using Resolve = WGLResolve;
};
#else
#    error "Only WIN32 is supported"
#endif
#pragma once

#ifdef _WIN32
#    include "WGL/WGLPhysicalDevice.h"
namespace OpenGL
{
    using Physicaldevice = WGLPhysicalDevice;
};
#else
#    error "Only WIN32 is supported"
#endif
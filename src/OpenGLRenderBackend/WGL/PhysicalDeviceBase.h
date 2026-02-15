#pragma once

#include "hrs/non_creatable.hpp"
#include "../Render.h"

namespace OpenGL
{
    class PhysicalDeviceBase : hrs::non_copyable, hrs::non_movable
    {
    public:
        PhysicalDeviceBase(Instance* instance);
        ~PhysicalDeviceBase();

        GLADloadfunc GetProcAddressResolver() const noexcept;

        const PhysicalDeviceSurfaceDesc& GetSurfaceDesc() const noexcept;
        const PhysicalDeviceSurfaceDesc GetSurfaceDescByIndex(std::uint32_t index) const noexcept;

        std::uint32_t GetDescribePixelFormatIndex(std::uint32_t index) const noexcept;

        void MakeCurrent();

        bool IsRobustContextSupported() const noexcept;
    protected:
        GladGLContext loader;
    private:
        HINSTANCE hinstance;
        //all dummy
        HWND window;
        HDC dc;
        HGLRC glrc;

        PhysicalDeviceSurfaceDesc surface_desc;
        std::vector<std::uint32_t> pixelformat_indices;
    };
};
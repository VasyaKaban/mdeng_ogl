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

        const Render::SurfaceCapabilities& GetSurfaceCapabilities() const noexcept;
        const Render::SurfaceCapabilities
        GetSurfaceCapabilitiesByIndex(std::uint32_t index) const noexcept;

        std::uint32_t GetDescribePixelFormatIndex(std::uint32_t index) const noexcept;

        void MakeCurrent();

        bool IsRobustContextSupported() const noexcept;
    private:
        HINSTANCE hinstance;
        //all dummy
        HWND window;
        HDC dc;
        HGLRC glrc;

        Render::SurfaceCapabilities surface_capabilities;
        std::vector<std::uint32_t> pixelformat_indices;
    };
};
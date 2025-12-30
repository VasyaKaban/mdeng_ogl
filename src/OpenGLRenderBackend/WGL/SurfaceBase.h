#pragma once

#include "OpenGLRenderBackend/Render.h"
#include "hrs/non_creatable.hpp"

namespace OpenGL
{
    class SurfaceBase : hrs::non_copyable, hrs::non_movable
    {
    public:
        SurfaceBase(const Render::SurfaceWin32Info& _info) noexcept;
        ~SurfaceBase();

        void Connect(const SurfaceConnectInfo& connect_info);
        bool IsConnected() const noexcept;

        void SetSwapInterval(Render::PresentModeFlagBits present_mode);
        void SwapWindow();
        void MakeCurrent();
        Render::Extent2D GetCurrentExtent() const noexcept;
    private:
        Render::SurfaceWin32Info info;
        HGLRC glrc; //real
    };
};
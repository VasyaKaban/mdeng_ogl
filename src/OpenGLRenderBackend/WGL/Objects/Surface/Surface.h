#pragma once

#include "hrs/non_creatable.hpp"
#include "Core/Render/Objects/Surface.h"
#include "../../../Render.h"

namespace OpenGL
{
    class Surface : public Render::Surface, hrs::non_copyable, hrs::non_movable
    {
    public:
        Surface(Instance* _parent, const Render::SurfaceWin32Info& info) noexcept;

        virtual ~Surface() override;

        virtual bool IsPresentable() const noexcept override;

        virtual Render::Instance* GetParent() const noexcept override;

        Render::SurfaceCapabilities GetConnectedCapabilities() const;
        PhysicalDevice* GetConnectedPhysicalDevice() const noexcept;

        void Connect(const SurfaceConnectInfo& info);

        bool IsConnected() const noexcept;

        void SetSwapInterval(Render::PresentModeFlagBits present_mode);
        void SwapWindow();
        void MakeCurrent();
    private:
        Instance* parent;
        Render::SurfaceWin32Info win32_info;

        HGLRC glrc; //real
        PhysicalDevice* connected_physical_device;

        Render::SurfaceCapabilities connected_capabilities;
    };
};
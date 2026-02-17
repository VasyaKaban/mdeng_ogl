#pragma once

#include "Core/Render/Objects/Surface.h"
#include "../../Instance/Instance.h"

namespace OpenGL
{
    class WGLSurface : public Render::Surface
    {
    public:
        WGLSurface(Instance* _parent, const Render::Win32SurfaceInfo& _info) noexcept;

        virtual ~WGLSurface() override;

        virtual Render::Instance* GetParent() const noexcept override;

        const Render::SurfaceCapabilities& GetConnectedCapabilities() const;
        PhysicalDevice* GetConnectedPhysicalDevice() const noexcept;

        void Connect(const SurfaceConnectInfo& connect_info);

        bool IsConnected() const noexcept;

        void SetSwapInterval(Render::PresentModeFlagBits present_mode);
        void SwapWindow();
        void MakeCurrent();
    private:
        HDC dc;
        HGLRC glrc; //real

        Instance* parent;

        PhysicalDevice* connected_physical_device;
        Render::SurfaceCapabilities connected_capabilities;
    };
};
#pragma once

#include "Core/Render/Objects/Surface.h"

#ifdef _WIN32
#    include "../../WGL/SurfaceBase.h"
#else
#    error "Only WIN32 is supported"
#endif

namespace OpenGL
{
    class Surface : public Render::Surface, private SurfaceBase
    {
    public:
#ifdef _WIN32
        Surface(Instance* _parent, const Render::SurfaceWin32Info& info) noexcept;
#endif

        virtual ~Surface() override;

        virtual Render::Instance* GetParent() const noexcept override;

        const Render::SurfaceCapabilities& GetConnectedCapabilities() const;
        PhysicalDevice* GetConnectedPhysicalDevice() const noexcept;

        void Connect(const SurfaceConnectInfo& info);

        bool IsConnected() const noexcept;

        void SetSwapInterval(Render::PresentModeFlagBits present_mode);
        void SwapWindow();
        void MakeCurrent();
    private:
        Instance* parent;

        PhysicalDevice* connected_physical_device;
        Render::SurfaceCapabilities connected_capabilities;
    };
};
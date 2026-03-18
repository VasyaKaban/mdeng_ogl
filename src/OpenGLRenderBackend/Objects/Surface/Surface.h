#pragma once

#include "Core/Render/Objects/Surface.h"
#include "../Instance/Instance.h"

namespace OpenGL
{
    class Surface final : public Render::Surface
    {
    public:
#ifdef _WIN32
        Surface(Instance* _parent, const Render::Win32SurfaceInfo& _info);
#elif defined(linux)
        Surface(Instance* _parent, const Render::XCBSurfaceInfo& _info);
        Surface(Instance* _parent, const Render::Wayland32SurfaceInfo& _info);
#endif
        virtual ~Surface() override;

        virtual Render::Instance* GetParent() const noexcept override;

        virtual const Render::LegacyPhysicalDeviceFeatures&
        GetLegacyPhysicalDeviceFeatures() const noexcept override;

        virtual Render::SurfaceCapabilities GetLegacySurfaceCapablities() const override;

        Device* GetConnectedDevice() const noexcept;

        void Connect(const Render::LegacyDeviceInfo& info);

        bool IsConnected() const noexcept;

        void SetSwapInterval(Render::PresentModeFlagBits present_mode);
        void SwapWindow();
        void MakeCurrent();

        GLADloadfunc GetProcAddressResolver() const noexcept;
    private:
        Instance* parent;
        Device* connected_device;
#ifdef _WIN32
        HWND window;
        HDC dc;
        HGLRC glrc; //as device inner handle but let it be inside surface like in WSI
#else
#    error TODO!
#endif
        std::vector<std::uint32_t> pixelformat_indices;
        Render::SurfaceCapabilities connected_capabilities;
        Render::LegacyPhysicalDeviceFeatures legacy_physical_device_features;
    };
};
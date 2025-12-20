#pragma once

#include "hrs/non_creatable.hpp"
#include "Core/Render/Objects/Surface.h"
#include "../../Render.h"

namespace OpenGL
{
    struct SurfaceConnectInfo
    {
        PhysicalDevice* physical_device;
        std::uint32_t config_index;
        bool robust_buffer_access_enabled;
    };

    class Surface : public Render::Surface, hrs::non_copyable, hrs::non_movable
    {
    public:
        Surface(Instance* _parent, const Render::SurfaceWin32Info& info) noexcept;

        virtual ~Surface() override;

        virtual Render::Instance* GetParent() const noexcept override;

        Render::SurfaceCapabilities GetConnectedCapabilities() const;
        PhysicalDevice* GetConnectedPhysicalDevice() const noexcept;

        void Connect(const SurfaceConnectInfo& info);

        bool IsConnected() const noexcept;

        void SetSwapInterval(Render::PresentModeFlagBits present_mode);
        void SwapWindow();
        std::span<Render::Image*> GetImages() noexcept;
        std::uint32_t GetImageIndex() const noexcept;

        void MakeCurrent();
    private:
        Instance* parent;
        Render::SurfaceWin32Info win32_info;

        HGLRC glrc; //real
        PhysicalDevice* connected_physical_device;

        Render::SurfaceCapabilities connected_capabilities;
        std::array<Render::Image*, SURFACE_IMAGE_COUNT> images;

        std::uint32_t image_index;
    };
};
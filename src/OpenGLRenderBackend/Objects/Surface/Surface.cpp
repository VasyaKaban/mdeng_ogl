#include "Surface.h"
#include "../Instance/Instance.h"
#include "../PhysicalDevice/PhysicalDevice.h"

namespace OpenGL
{
#ifdef _WIN32
    Surface::Surface(Instance* _parent, const Render::SurfaceWin32Info& info) noexcept
        : SurfaceBase(info),
          parent(_parent),
          connected_physical_device(nullptr)
    {}
#endif

    Surface::~Surface()
    {}

    Render::Instance* Surface::GetParent() const noexcept
    {
        return parent;
    }

    const Render::SurfaceCapabilities& Surface::GetConnectedCapabilities() const
    {
        return connected_capabilities;
    }

    PhysicalDevice* Surface::GetConnectedPhysicalDevice() const noexcept
    {
        return connected_physical_device;
    }

    void Surface::Connect(const SurfaceConnectInfo& info)
    {
        this->SurfaceBase::Connect(info);

        auto surface_desc = static_cast<PhysicalDevice*>(info.physical_device)
                                ->GetSurfaceDescByIndex(info.format_index);

        connected_capabilities = Render::SurfaceCapabilities{
            .min_image_count = SURFACE_MIN_IMAGE_COUNT,
            .max_image_count = SURFACE_MAX_IMAGE_COUNT,
            .supported_present_modes = surface_desc.supported_present_modes,
            .supported_formats = surface_desc.supported_formats,
            .min_extent = SURFACE_MIN_EXTENT,
            .current_extent = SURFACE_CURRENT_EXTENT,
            .max_extent = SURFACE_MAX_EXTENT,
            .extent_mode = SURFACE_EXTENT_MODE};

        connected_physical_device = info.physical_device;
    }

    bool Surface::IsConnected() const noexcept
    {
        return this->SurfaceBase::IsConnected();
    }

    void Surface::SetSwapInterval(Render::PresentModeFlagBits present_mode)
    {
        this->SurfaceBase::SetSwapInterval(present_mode);
    }

    void Surface::SwapWindow()
    {
        this->SurfaceBase::SwapWindow();
    }

    void Surface::MakeCurrent()
    {
        this->SurfaceBase::MakeCurrent();
    }
};
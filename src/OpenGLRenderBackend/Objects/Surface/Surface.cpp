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

    bool Surface::IsPresentable() const noexcept
    {
        return true; //in OpenGL we always have presentable contexts???
    }

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

        connected_capabilities = static_cast<PhysicalDevice*>(info.physical_device)
                                     ->GetSurfaceCapabilitiesByIndex(info.config_index);

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
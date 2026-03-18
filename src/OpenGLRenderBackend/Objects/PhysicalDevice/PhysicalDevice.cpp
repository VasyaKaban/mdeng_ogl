#include "PhysicalDevice.h"
#include <stdexcept>
#include <map>
#include "Core/Utils/Expected.hpp"
#include "Core/Utils/ScopedCall.hpp"
#include "../Instance/Instance.h"
#include "../Device/Device.h"
#include "../Surface/Surface.h"
#include "glad/wgl.h"
#include "Core/Utils/System.h"
#include "Core/Render/Format.h"

namespace OpenGL
{
    PhysicalDevice::PhysicalDevice(Instance* _parent, Device* _device)
        : parent(_parent),
          device(_device)
    {
        properties = GetPhysicalDeviceProperties(device->GetLoader(),
                                                 GLAD_WGL_ARB_create_context_robustness != 0);
    }

    PhysicalDevice::~PhysicalDevice()
    {}

    const Render::PhysicalDeviceProperties& PhysicalDevice::GetProperties() const noexcept
    {
        return properties;
    }

    bool PhysicalDevice::GetSurfaceSupport(Render::Surface* surface,
                                           std::uint32_t queue_family_index) const noexcept
    {
        Surface* impl_surface = static_cast<Surface*>(surface);

        return impl_surface->IsConnected() && impl_surface->GetConnectedDevice() == device;
    }

    Render::SurfaceCapabilities
    PhysicalDevice::GetSurfaceCapablities(Render::Surface* surface) const
    {
        if(!GetSurfaceSupport(surface, 0))
            throw std::runtime_error("Legacy physical device does not support other surfaces");

        Surface* impl_surface = static_cast<Surface*>(surface);
        return impl_surface->GetLegacySurfaceCapablities();
    }

    std::optional<Render::BufferFormatProperties>
    PhysicalDevice::GetBufferFormatProperties(const Render::BufferFormatInfo& info) const
    {
        return GetPhysicalDeviceBufferFormatProperties(device->GetLoader(), info);
    }

    std::optional<Render::ImageFormatProperties>
    PhysicalDevice::GetImageFormatProperties(const Render::ImageFormatInfo& info) const
    {
        return GetPhysicalDeviceImageFormatProperties(device->GetLoader(), info);
    }

    Render::Device* PhysicalDevice::CreateDevice(const Render::DeviceInfo& info)
    {
        throw std::runtime_error("Legacy physical device does not support device creation");
    }

    Render::Instance* PhysicalDevice::GetParent() const noexcept
    {
        return parent;
    }
};
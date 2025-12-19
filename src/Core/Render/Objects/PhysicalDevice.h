#pragma once

#include <optional>
#include "../Render.h"

namespace Render
{
    class PhysicalDevice
    {
    public:
        virtual ~PhysicalDevice() {};

        virtual const PhysicalDeviceProperties& GetProperties() const noexcept = 0;

        virtual bool GetSurfaceSupport(Surface* surface) const noexcept = 0;

        virtual SurfaceCapabilities GetSurfaceCapablities(Surface* surface) const noexcept = 0;

        virtual std::optional<BufferFormatProperties>
        GetBufferFormatProperties(const BufferFormatInfo& info) const = 0;

        virtual std::optional<ImageFormatProperties>
        GetImageFormatProperties(const ImageFormatInfo& info) const = 0;

        virtual Device* CreateDevice(const DeviceInfo& info) = 0;

        virtual Instance* GetParent() const noexcept = 0;
    };
};
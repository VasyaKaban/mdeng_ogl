#pragma once

#include <optional>
#include "Core/Render/Objects/PhysicalDevice.h"
#include "../../Render.h"

namespace OpenGL
{
    class PhysicalDevice final : public Render::PhysicalDevice
    {
    public:
        PhysicalDevice(Instance* _parent, Device* _device);
        virtual ~PhysicalDevice() override;

        virtual const Render::PhysicalDeviceProperties& GetProperties() const noexcept override;

        virtual bool GetSurfaceSupport(Render::Surface* surface,
                                       std::uint32_t queue_family_index) const noexcept override;

        virtual Render::SurfaceCapabilities
        GetSurfaceCapablities(Render::Surface* surface) const override;

        virtual std::optional<Render::BufferFormatProperties>
        GetBufferFormatProperties(const Render::BufferFormatInfo& info) const override;

        virtual std::optional<Render::ImageFormatProperties>
        GetImageFormatProperties(const Render::ImageFormatInfo& info) const override;

        virtual Render::Device* CreateDevice(const Render::DeviceInfo& info) override;

        virtual Render::Instance* GetParent() const noexcept override;
    private:
        Instance* parent;
        Device* device;
        Render::PhysicalDeviceProperties properties;
    };
};
#pragma once

#include <optional>
#include "hrs/non_creatable.hpp"
#include "Core/Render/Objects/PhysicalDevice.h"
#include "../../Render.h"

#ifdef _WIN32
#    include "../../WGL/PhysicalDeviceBase.h"
#else
#    error "Only WIN32 is supported"
#endif

namespace OpenGL
{
    class PhysicalDevice : public Render::PhysicalDevice, private PhysicalDeviceBase
    {
    public:
        PhysicalDevice(Instance* _parent);
        virtual ~PhysicalDevice() override;

        virtual const Render::PhysicalDeviceProperties& GetProperties() const noexcept override;

        virtual bool GetSurfaceSupport(Render::Surface* surface,
                                       std::uint32_t queue_family_index) const noexcept override;

        virtual Render::SurfaceCapabilities
        GetSurfaceCapablities(Render::Surface* surface) const noexcept override;

        virtual std::optional<Render::BufferFormatProperties>
        GetBufferFormatProperties(const Render::BufferFormatInfo& info) const override;

        virtual std::optional<Render::ImageFormatProperties>
        GetImageFormatProperties(const Render::ImageFormatInfo& info) const override;

        virtual Render::Device* CreateDevice(const Render::DeviceInfo& info) override;

        virtual Render::Instance* GetParent() const noexcept override;

        GLADloadfunc GetProcAddressResolver() const noexcept;

        const Render::SurfaceCapabilities
        GetSurfaceCapabilitiesByIndex(std::uint32_t index) const noexcept;

        std::uint32_t GetDescribePixelFormatIndex(std::uint32_t index) const noexcept;

        void DeleteDeviceNotify() noexcept;

        void SetDebugMessenger(const Render::DebugMessengerInfo& info);
    private:
        Instance* parent;

        GladGLContext loader;
        Render::PhysicalDeviceProperties properties;

        Device* device;
    };
};
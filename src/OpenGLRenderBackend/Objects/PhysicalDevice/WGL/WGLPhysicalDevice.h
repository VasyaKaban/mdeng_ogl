#pragma once

#include <optional>
#include "Core/Render/Objects/PhysicalDevice.h"
#include "../../../Render.h"

namespace OpenGL
{
    class WGLPhysicalDevice : public Render::PhysicalDevice
    {
    public:
        WGLPhysicalDevice(Instance* _parent);
        virtual ~WGLPhysicalDevice() override;

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

        Render::PresentModeFlags GetSupportedPresentModes() const noexcept;
        std::uint32_t GetDescribePixelFormatIndex(Render::Format format) const;

        void DeleteDeviceNotify() noexcept;

        void SetDebugMessenger(const Render::DebugMessengerInfo& info);
    private:
        HINSTANCE hinstance;
        //all dummy
        HWND window;
        HDC dc;
        HGLRC glrc;

        GladGLContext loader;

        PhysicalDeviceSurfaceDesc surface_desc;
        std::vector<std::uint32_t> pixelformat_indices;

        Instance* parent;
        Render::PhysicalDeviceProperties properties;

        Device* device;
    };
};
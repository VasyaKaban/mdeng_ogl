#pragma once

#include <optional>
#include "hrs/non_creatable.hpp"
#include "Core/Render/Objects/PhysicalDevice.h"
#include "../../Render.h"

namespace OpenGL
{
    class PhysicalDevice : public Render::PhysicalDevice, hrs::non_copyable, hrs::non_movable
    {
    public:
        PhysicalDevice(Instance* _parent);
        virtual ~PhysicalDevice() override;

        virtual const Render::PhysicalDeviceProperties& GetProperties() const noexcept override;

        virtual bool GetSurfaceSupport(Render::Surface* surface) const noexcept override;

        virtual Render::SurfaceCapabilities
        GetSurfaceCapablities(Render::Surface* surface) const noexcept override;

        virtual std::optional<Render::BufferFormatProperties>
        GetBufferFormatProperties(const Render::BufferFormatInfo& info) const override;

        virtual std::optional<Render::ImageFormatProperties>
        GetImageFormatProperties(const Render::ImageFormatInfo& info) const override;

        virtual Render::Device* CreateDevice(const Render::DeviceInfo& info) override;

        virtual Render::Instance* GetParent() const noexcept override;

        GLADloadfunc GetProcAddressResolver() const noexcept;

        Render::SurfaceCapabilities
        GetSurfaceCapabilitiesByIndex(std::uint32_t index) const noexcept;

        void DeleteDeviceNotify() noexcept;

        void SetDebugMessenger(const Render::DebugMessengerInfo& info);
    private:
        Instance* parent;

        HINSTANCE hinstance;
        //all dummy
        HWND window;
        HDC dc;
        HGLRC glrc;

        GladGLContext loader;

        Render::PhysicalDeviceProperties properties;
        Render::SurfaceCapabilities surface_capabilities;
        std::vector<std::uint32_t> pixelformat_indices;

        Device* device;
    };
};
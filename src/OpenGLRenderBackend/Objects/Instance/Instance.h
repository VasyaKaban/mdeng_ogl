#pragma once

#include "Core/Render/Objects/Instance.h"
#include "Core/Utils/NonCreatable.hpp"
#include "../../Render.h"

namespace OpenGL
{
    class Instance final : public Render::Instance, Core::NonCopyable, Core::NonMovable
    {
    public:
        Instance(const Render::InstanceInfo& info);
        virtual ~Instance() override;

        virtual Render::ContextMode GetContextMode() const noexcept override;

        virtual Render::Device* CreateLegacyDevice(const Render::LegacyDeviceInfo& info) override;

        virtual std::vector<Render::PhysicalDevice*> GetPhysicalDevices() const override;

#ifdef _WIN32
        virtual Render::Surface* CreateSurface(const Render::Win32SurfaceInfo& info) override;
#elif defined(linux)
        virtual Surface* CreateSurface(const XCBSurfaceInfo& info) override;
        virtual Surface* CreateSurface(const WaylandSurfaceInfo& info) override;
#endif

        const Render::InstanceFeatures& GetEnabledFeatures() const noexcept;
        const Render::DebugMessengerInfo& GetDebugMessengerInfo() const noexcept;
    private:
        Render::InstanceFeatures enabled_features;
        Render::DebugMessengerInfo debug_messenger_info;
    };
};
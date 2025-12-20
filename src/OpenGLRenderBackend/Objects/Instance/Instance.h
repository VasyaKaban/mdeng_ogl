#pragma once

#include "Core/Render/Objects/Instance.h"
#include "hrs/non_creatable.hpp"
#include "../../Render.h"

namespace OpenGL
{
    class Instance : public Render::Instance, hrs::non_copyable, hrs::non_movable
    {
    public:
        Instance(const Render::InstanceInfo& info);
        virtual ~Instance() override;

        virtual std::vector<Render::PhysicalDevice*> GetPhysicalDevices() const override;

        virtual Render::Surface* CreateSurface(const Render::SurfaceWin32Info& info) override;

        virtual void SetDebugMessenger(const Render::DebugMessengerInfo& info) override;

        const Render::InstanceFeatures& GetEnabledFeatures() const noexcept;
        const Render::DebugMessengerInfo& GetDebugMessengerInfo() const noexcept;
    private:
        std::vector<PhysicalDevice*> physical_devices;
        Render::InstanceFeatures enabled_features;
        Render::DebugMessengerInfo debug_messenger_info;
    };
};
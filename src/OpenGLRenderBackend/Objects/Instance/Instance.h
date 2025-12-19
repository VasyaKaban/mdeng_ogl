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

        virtual std::vector<const Render::PhysicalDevice*> GetPhysicalDevices() const override;

        virtual Render::Surface* CreateSurface(const Render::SurfaceWin32Info& info) override;
    private:
        std::vector<Render::PhysicalDevice*> physical_devices;
    };
};
#include "Instance.h"
#include "Core/Render/Objects/PhysicalDevice.h"
#include "../Surface/Surface.h"
#include "../PhysicalDevice/PhysicalDevice.h"

namespace OpenGL
{
    Instance::Instance(const Render::InstanceInfo& info)
    {
        physical_devices.push_back(new PhysicalDevice(this)); //in OGL we have only one device
    }

    Instance::~Instance()
    {
        for(auto& dev: physical_devices)
            delete dev;
    }

    std::vector<const Render::PhysicalDevice*> Instance::GetPhysicalDevices() const
    {
        std::vector<const Render::PhysicalDevice*> out;
        out.reserve(physical_devices.size());
        for(auto& dev: physical_devices)
            out.push_back(dev);

        return out;
    }

    Render::Surface* Instance::CreateSurface(const Render::SurfaceWin32Info& info)
    {
        return new Surface(info);
    }
};
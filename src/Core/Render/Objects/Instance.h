#pragma once

#include "../Render.h"

namespace Render
{
    class Instance
    {
    public:
        virtual ~Instance() {};

        virtual std::vector<const PhysicalDevice*> GetPhysicalDevices() const = 0;

        virtual Surface* CreateSurface(const SurfaceWin32Info& info) = 0;
    };
};
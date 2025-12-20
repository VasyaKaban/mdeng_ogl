#pragma once

#include "../Render.h"

namespace Render
{
    class Instance
    {
    public:
        virtual ~Instance() {};

        virtual std::vector<PhysicalDevice*> GetPhysicalDevices() const = 0;

        virtual Surface* CreateSurface(const SurfaceWin32Info& info) = 0;

        virtual void SetDebugMessenger(const DebugMessengerInfo& info) = 0;
    };
};
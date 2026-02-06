#pragma once

#include "../Render.h"

namespace Render
{
    class CORE_API Instance
    {
    public:
        virtual ~Instance() = 0;

        virtual std::vector<PhysicalDevice*> GetPhysicalDevices() const = 0;

#ifdef _WIN32
        virtual Surface* CreateSurface(const SurfaceWin32Info& info) = 0;
#endif

        virtual void SetDebugMessenger(const DebugMessengerInfo& info) = 0;
    };
};
#pragma once

#include "../Render.h"

namespace Render
{
    class CORE_API Instance
    {
    public:
        virtual ~Instance() = 0;

        virtual ContextMode GetContextMode() const noexcept = 0;

        virtual Device* CreateLegacyDevice(const LegacyDeviceInfo& info) = 0;

        virtual std::vector<PhysicalDevice*> GetPhysicalDevices() const = 0;

#ifdef _WIN32
        virtual Surface* CreateSurface(const Win32SurfaceInfo& info) = 0;
#elif defined(linux)
        virtual Surface* CreateSurface(const XCBSurfaceInfo& info) = 0;
        virtual Surface* CreateSurface(const WaylandSurfaceInfo& info) = 0;
#endif
    };
};
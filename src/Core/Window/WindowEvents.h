#pragma once

#include <type_traits>
#include "Core/Utils/ClassID.hpp"
#include "Core/API.h"

namespace Core
{
    class Display;

    struct WindowResolution
    {
        std::uint32_t width;
        std::uint32_t height;
    };

    struct WindowPosition
    {
        std::int32_t x;
        std::int32_t y;
    };

    struct WindowSubsystemQuitEvent
    {};
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowSubsystemQuitEvent>;

    struct WindowCloseEvent
    {};
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowCloseEvent>;

    struct WindowResizedEvent
    {
        WindowResolution resolution;
        WindowResolution scaled_resolution;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowResizedEvent>;

    struct WindowMovedEvent
    {
        WindowPosition position; //global
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowMovedEvent>;

    struct WindowDisplayChangedEvent
    {
        Display* display;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowDisplayChangedEvent>;
};
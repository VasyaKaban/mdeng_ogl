#pragma once

#include <type_traits>
#include "Core/Utils/ClassID.hpp"
#include "Core/API.h"

namespace Core
{
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

    struct WindowScaleChangedEvent
    {
        float scale_factor;
        float surface_scale_factor;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowScaleChangedEvent>;

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
        WindowPosition position;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowMovedEvent>;

    enum MouseButtonBits : std::uint32_t
    {
        Left = 1u << 0,
        Middle = 1u << 1,
        Right = 1u << 2,
        X1 = 1u << 2,
        X2 = 1u << 3
    };

    using MouseButtonMask = std::underlying_type_t<MouseButtonBits>;

    struct MouseMotionEvent
    {
        MouseButtonMask mask;
        std::int32_t x;
        std::int32_t y;
        std::int32_t motion_x;
        std::int32_t motion_y;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<MouseMotionEvent>;

    enum class ButtonState
    {
        Pressed,
        Released
    };

    struct MouseButtonEvent
    {
        MouseButtonBits button;
        ButtonState state;
        std::uint8_t clicks;
        std::int32_t x;
        std::int32_t y;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<MouseButtonEvent>;

    struct MouseWheelEvent
    {
        std::int32_t scrolled_x;
        std::int32_t scrolled_y;
        std::int32_t x;
        std::int32_t y;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<MouseWheelEvent>;
};
#pragma once

#include <type_traits>
#include "Core/Utils/ClassID.hpp"
#include "Core/API.h"

namespace Core
{
    struct WindowResolution
    {
        int width;
        int height;
    };

    struct WindowPosition
    {
        int x;
        int y;
    };

    struct WindowCloseEvent
    {};
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowCloseEvent>;

    struct WindowResizedEvent
    {
        WindowResolution resolution;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowResizedEvent>;

    struct WindowExposedEvent
    {};
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowExposedEvent>;

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
        X2 = 1u << 3,
        X3 = 1u << 4,
        X4 = 1u << 5,
        X5 = 1u << 6,
        X6 = 1u << 7,
        X7 = 1u << 8,
        X8 = 1u << 9,
        X9 = 1u << 10,
        X10 = 1u << 11,
        X11 = 1u << 12,
        X12 = 1u << 13,
        X13 = 1u << 14,
        X14 = 1u << 15,
        X15 = 1u << 16,
        X16 = 1u << 17,
        X17 = 1u << 18,
        X18 = 1u << 19,
        X19 = 1u << 20,
        X20 = 1u << 21,
        X21 = 1u << 22,
        X22 = 1u << 23,
        X23 = 1u << 24,
        X24 = 1u << 25,
        X25 = 1u << 26,
        X26 = 1u << 27,
        X27 = 1u << 28,
        X28 = 1u << 29,
        X29 = 1u << 30,
        X30 = 1u << 31
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
        //Uint32 direction;   /**< Set to one of the SDL_MOUSEWHEEL_* defines. When FLIPPED the values in X and Y will be opposite. Multiply by -1 to change them back */
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<MouseWheelEvent>;

    //SDL_WINDOWEVENT_SHOWN,          /**< Window has been shown */
    //SDL_WINDOWEVENT_HIDDEN,         /**< Window has been hidden */

    //SDL_WINDOWEVENT_SIZE_CHANGED,   /**< The window size has changed, either as
    //                                     a result of an API call or through the
    //                                     system or user changing the window size. */
    //SDL_WINDOWEVENT_MINIMIZED,      /**< Window has been minimized */
    //SDL_WINDOWEVENT_MAXIMIZED,      /**< Window has been maximized */
    //SDL_WINDOWEVENT_RESTORED,       /**< Window has been restored to normal size
    //                                     and position */
    //SDL_WINDOWEVENT_ENTER,          /**< Window has gained mouse focus */
    //SDL_WINDOWEVENT_LEAVE,          /**< Window has lost mouse focus */
    //SDL_WINDOWEVENT_FOCUS_GAINED,   /**< Window has gained keyboard focus */
    //SDL_WINDOWEVENT_FOCUS_LOST,     /**< Window has lost keyboard focus */
    //SDL_WINDOWEVENT_TAKE_FOCUS,     /**< Window is being offered a focus (should SetWindowInputFocus() on itself or a subwindow, or ignore) */
    //SDL_WINDOWEVENT_HIT_TEST,       /**< Window had a hit test that wasn't SDL_HITTEST_NORMAL. */
    //SDL_WINDOWEVENT_ICCPROF_CHANGED,/**< The ICC profile of the window's display has changed. */
    //SDL_WINDOWEVENT_DISPLAY_CHANGED /**< Window has been moved to display data1. */
};
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

    enum MouseButtonFlagBits : std::uint32_t
    {
        LeftButton = 1 << 0,
        MiddleButton = 1 << 1,
        RightButton = 1 << 2,
        X1Button = 1 << 3,
        X2Button = 1 << 4,
    };

    using MouseButtonFlags = std::underlying_type_t<MouseButtonFlagBits>;

    struct WindowSubsystemQuitEvent
    {
        std::uint64_t timestamp_ms;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowSubsystemQuitEvent>;

    struct WindowCloseEvent
    {
        std::uint64_t timestamp_ms;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowCloseEvent>;

    struct WindowDisplayChangedEvent
    {
        std::uint64_t timestamp_ms;
        Display* display;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowDisplayChangedEvent>;

    struct WindowMovedEvent
    {
        std::uint64_t timestamp_ms;
        WindowPosition position;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowMovedEvent>;

    struct WindowResizedEvent
    {
        std::uint64_t timestamp_ms;
        WindowResolution resolution;
        WindowResolution scaled_resolution;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowResizedEvent>;

    struct WindowMinimizedEvent
    {
        std::uint64_t timestamp_ms;
        WindowResolution resolution;
        WindowResolution scaled_resolution;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowMinimizedEvent>;

    struct WindowMaximizedEvent
    {
        std::uint64_t timestamp_ms;
        WindowResolution resolution;
        WindowResolution scaled_resolution;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowMaximizedEvent>;

    struct WindowHiddenEvent
    {
        std::uint64_t timestamp_ms;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowHiddenEvent>;

    struct WindowShownEvent
    {
        std::uint64_t timestamp_ms;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowShownEvent>;

    struct WindowCursorFocusGainEvent
    {
        std::uint64_t timestamp_ms;
        MouseButtonFlags buttons;
        WindowPosition cursor_position;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowCursorFocusGainEvent>;

    struct WindowCursorFocusLeaveEvent
    {
        std::uint64_t timestamp_ms;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowCursorFocusLeaveEvent>;

    struct WindowKeyboardFocusGainEvent
    {
        std::uint64_t timestamp_ms;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowKeyboardFocusGainEvent>;

    struct WindowKeyboardFocusLeaveEvent
    {
        std::uint64_t timestamp_ms;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowKeyboardFocusLeaveEvent>;

    struct MouseButtonPressedEvent
    {
        std::uint64_t timestamp_ms;
        MouseButtonFlagBits button;
        std::uint32_t clicks;
        WindowPosition cursor_position;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<MouseButtonPressedEvent>;

    struct MouseButtonReleasedEvent
    {
        std::uint64_t timestamp_ms;
        MouseButtonFlagBits button;
        WindowPosition cursor_position;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<MouseButtonReleasedEvent>;

    struct MouseCursorMoveEvent
    {
        std::uint64_t timestamp_ms;
        MouseButtonFlags buttons;
        WindowPosition cursor_position;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<MouseCursorMoveEvent>;

    struct MouseWheelEvent
    {
        std::uint64_t timestamp_ms;
        MouseButtonFlags buttons;
        WindowPosition cursor_position;
        float x_scroll; //positive -> right, negative -> left
        float y_scroll; //positive -> away from user, negative -> toward the user
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<MouseWheelEvent>;

    //Quit
    //Close
    //DisplayChanged
    //Moved
    //Resized
    //Minimized
    //Maximized
    //Hide
    //Show
    //CursorFocusGain
    //CursorFocusLeave
    //KeyboardFocusGain
    //KeyboardFocusLeave
    //MouseButtonPressedEvent
    //MouseButtonReleasedEvent
    //MouseCursorMoveEvent
    //MouseWheelEvent
    //TODO:
    //KeyPressedEvent: scancode + special key(enum)/char
    //KeyReleasedEvent: scancode
};
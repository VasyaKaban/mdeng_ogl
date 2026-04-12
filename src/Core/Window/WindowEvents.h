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

    struct WindowSybsystemKeyboardLayoutChangedEvent
    {
        std::uint64_t timestamp_ms;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowSybsystemKeyboardLayoutChangedEvent>;

    struct WindowClosedEvent
    {
        std::uint64_t timestamp_ms;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowClosedEvent>;

    struct WindowDisplayChangedEvent
    {
        std::uint64_t timestamp_ms;
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

    using ScanCode = std::uint16_t;

    enum ModifierKeyFlagBits : std::uint32_t
    {
        //TODO
        LeftShift = 1 << 0,
        RightShift = 1 << 1,
        LeftControl = 1 << 2,
        RightControl = 1 << 3,
        LeftAlt = 1 << 4,
        RightAlt = 1 << 5,
        LeftMeta = 1 << 6, //Win
        RightMeta = 1 << 7
    };

    using ModifierKeyFlags = std::underlying_type_t<ModifierKeyFlagBits>;

    using RawKeyCode = std::uint32_t; //raw key code(Virtual key on Win32 and KeySym on XKB)

    struct KeyboardKeyPressedEvent
    {
        std::uint64_t timestamp_ms;
        ScanCode scancode;
        RawKeyCode raw_key;
        ModifierKeyFlags modifiers;
        std::uint16_t repeat_count;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<KeyboardKeyPressedEvent>;

    struct KeyboardCharacterPressedEvent
    {
        std::uint64_t timestamp_ms;
        ModifierKeyFlags modifiers;
        std::uint16_t repeat_count;
        char32_t utf32_char;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<KeyboardCharacterPressedEvent>;

    struct KeyboardKeyReleasedEvent
    {
        std::uint64_t timestamp_ms;
        ScanCode scancode;
        RawKeyCode raw_key;
        ModifierKeyFlags modifiers;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<KeyboardKeyReleasedEvent>;

    //helper for inner subsystem event queue
    union QueueEvent
    {
        WindowSybsystemKeyboardLayoutChangedEvent window_sybsystem_keyboard_layout_changed;
        WindowClosedEvent window_closed;
        WindowDisplayChangedEvent window_display_changed;
        WindowMovedEvent window_moved;
        WindowResizedEvent window_resized;
        WindowMinimizedEvent window_minimized;
        WindowMaximizedEvent window_maximized;
        WindowHiddenEvent window_hidden;
        WindowShownEvent window_shown;
        WindowCursorFocusGainEvent window_cursor_focus_gain;
        WindowCursorFocusLeaveEvent window_cursor_focus_leave;
        WindowKeyboardFocusGainEvent window_keyboard_focus_gain;
        WindowKeyboardFocusLeaveEvent window_keyboard_focus_leave;
        MouseButtonPressedEvent mouse_button_pressed;
        MouseButtonReleasedEvent mouse_buttton_released;
        MouseCursorMoveEvent mouse_cursor_move;
        MouseWheelEvent mouse_wheel;
        KeyboardKeyPressedEvent keyboard_key_pressed;
        KeyboardCharacterPressedEvent keyboard_character_pressed;
        KeyboardKeyReleasedEvent keyboard_key_released;
    };

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
    //KeyboardKeyPressedEvent
    //KeyboardCharacterPressedEvent
    //KeyboardKeyReleasedEvent

    std::uint64_t GetEventTimestamp() noexcept;
};
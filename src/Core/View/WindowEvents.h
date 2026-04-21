#pragma once

#include "View.h"
#include "Core/Utils/ClassID.hpp"
#include "Core/API.h"

namespace Core
{
    class Display;

    //WindowSubsystem events
    struct DisplayAddedEvent
    {
        std::uint64_t timestamp_ms;
        std::shared_ptr<Display>
            display; //due to the 'flutter' nature of the display object, this event will be connected to WindowSubsystem. Other Display events will belong to the Display object itself
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<DisplayAddedEvent>;

    //Display events
    struct DisplayRemovedEvent
    {
        std::uint64_t timestamp_ms;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<DisplayRemovedEvent>;

    struct DisplayMovedEvent
    {
        std::uint64_t timestamp_ms;
        WindowPosition position;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<DisplayMovedEvent>;

    struct DisplayVideoModeChangedEvent
    {
        std::uint64_t timestamp_ms;
        VideoMode video_mode;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<DisplayVideoModeChangedEvent>;

    struct DisplayScaleChangedEvent
    {
        std::uint64_t timestamp_ms;
        float scale_factor;
        float display_scale_factor;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<DisplayScaleChangedEvent>;

    //Window events
    struct WindowClosedEvent
    {
        std::uint64_t timestamp_ms;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<WindowClosedEvent>;

    struct WindowDisplayChangedEvent
    {
        std::uint64_t timestamp_ms;
        std::shared_ptr<Display> display;
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

    struct KeyboardKeyPressedEvent
    {
        std::uint64_t timestamp_ms;
        ScanCode scancode;
        KeyboardKey
            key; //One of the special keys from SpecialKey or valid UTF32 character or SpecialKey::Unknown instead
        ModifierKeyFlags modifiers;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<KeyboardKeyPressedEvent>;

    struct KeyboardCharacterPressedEvent
    {
        std::uint64_t timestamp_ms;
        ModifierKeyFlags modifiers;
        char32_t utf32_char;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<KeyboardCharacterPressedEvent>;

    struct KeyboardKeyReleasedEvent
    {
        std::uint64_t timestamp_ms;
        ScanCode scancode;
        KeyboardKey key;
        ModifierKeyFlags modifiers;
    };
    CORE_API_TEMPLATE template class CORE_API ClassID<KeyboardKeyReleasedEvent>;

    //helper for inner subsystem event queue
    union QueueEvent
    {
        DisplayAddedEvent display_added;
        DisplayRemovedEvent display_removed;
        DisplayMovedEvent display_moved;
        DisplayVideoModeChangedEvent display_video_mode_changed;
        DisplayScaleChangedEvent display_scale_changed;
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
        MouseButtonReleasedEvent mouse_button_released;
        MouseCursorMoveEvent mouse_cursor_move;
        MouseWheelEvent mouse_wheel;
        KeyboardKeyPressedEvent keyboard_key_pressed;
        KeyboardCharacterPressedEvent keyboard_character_pressed;
        KeyboardKeyReleasedEvent keyboard_key_released;
    };

    std::uint64_t GetEventTimestamp() noexcept;
};
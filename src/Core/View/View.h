#pragma once

#include <cstdint>
#include <type_traits>
#include <string_view>
#include "Core/Render/Render.h"

namespace Core
{
    class WindowSubsystem;
    class Window;
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

    using KeyboardKey = std::uint32_t;

    //Let's take Private Use Area: U+E000..U+F8FF
    namespace SpecialKey
    {
        enum : KeyboardKey
        {
            Unknown = 0,
            Enter = 0xE0'00,
            Escape,
            BackSpace,
            Tab,
            Space,
            CapsLock,
            F1,
            F2,
            F3,
            F4,
            F5,
            F6,
            F7,
            F8,
            F9,
            F10,
            F11,
            F12,
            PrintScreen,
            SysRq,
            ScrollLock,
            Break,
            Pause,
            Insert,
            Home,
            PageUp,
            Delete,
            End,
            PageDown,
            RightArrow,
            LeftArrow,
            DownArrow,
            UpArrow,
            NumLock,
            KeypadDivide,
            KeypadMultiply,
            KeypadSubtract,
            KeypadAdd,
            KeypadEnter,
            Keypad1,
            Keypad2,
            Keypad3,
            Keypad4,
            Keypad5,
            Keypad6,
            Keypad7,
            Keypad8,
            Keypad9,
            Keypad0,
            KeypadPeriod,
            Menu,
            KeyboardPower,
            KeypadEqual,
            F13,
            F14,
            F15,
            F16,
            F17,
            F18,
            F19,
            F20,
            F21,
            F22,
            F23,
            F24,
            BrazilKeypad,
            Ro, //Keyboard Int'l 1
            Kana, //Keyboard Intl'2
            Yen, //Keyboard Int'l 3
            Henkan, //Keyboard Int'l 4
            Muhenkan, //Keyboard Int'l 5
            PC9800KeypadPeriod, //Keyboard Int'l 6
            Hanguel, //Keyboard Lang 1
            Hanja, //Keyboard Lang 2
            Katakana, //Keyboard Lang 3
            Hiragana, //Keyboard Lang 4
            ZenkakuHankaku, //Keyboard Lang 5
            LeftControl,
            LeftShift,
            LeftAlt,
            LeftGUI,
            RightControl,
            RightShift,
            RightAlt,
            RightGUI,
            ScanNextTrack,
            ScanPreviousTrack,
            Stop,
            PlayPause,
            Mute,
            VolumeUp,
            VolumeDown,
            MediaSelect,
            Mail,
            Calculator,
            MyComputer,
            WWWSearch,
            WWWHome,
            WWWBack,
            WWWForward,
            WWWStop,
            WWWRefresh,
            WWWFavorites
        };
    };

    using ScanCode = std::uint32_t;

    enum ModifierKeyFlagBits : std::uint32_t
    {
        //TODO
        LeftShift = 1 << 0,
        RightShift = 1 << 1,
        LeftControl = 1 << 2,
        RightControl = 1 << 3,
        LeftAlt = 1 << 4,
        RightAlt = 1 << 5,
        LeftGUI = 1 << 6, //Win
        RightGUI = 1 << 7
    };

    using ModifierKeyFlags = std::underlying_type_t<ModifierKeyFlagBits>;

    struct VideoMode
    {
        std::uint32_t width;
        std::uint32_t height;
        std::uint32_t refresh_rate;
        std::uint32_t bits_per_pixel;
    };

    enum class WindowState
    {
        Windowed = 0,
        FullScreen = 1
    };

    enum class WindowVisibility
    {
        Hidden = 0,
        Shown = 1
    };

    enum class CursorState
    {
        Enabled,
        Disbaled
    };

    struct WindowInfo
    {
        WindowResolution resolution;
        WindowState state;
        std::string_view title;
    };

    using WindowSurfaceInfo = std::variant<
#ifdef _WIN32
        Render::Win32SurfaceInfo
#elif defined(linux)
        Render::XCBSurfaceInfo
#endif
        >;

    enum class WindowSubsystemType
    {
#ifdef _WIN32
        Win32 = 0
#elif defined(linux)
        XCB = 1
#endif
    };

    struct WindowSubsystemInfo
    {
        WindowSubsystemType type;
    };

    CORE_API std::vector<WindowSubsystemType> GetAvailableWindowSubsystemTypes();

    CORE_API WindowSubsystem*
    CreateWindowSubsystem(const WindowSubsystemInfo& info); //may be ref-counted(and should be :) )

    CORE_API std::string KeyboardKeyToString(KeyboardKey key);
};
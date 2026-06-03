#pragma once

#include <cstdint>
#include <string_view>
#include <vector>
#include <string>
#include "Core/API.h"

namespace Core
{
    class WindowSubsystemConnection;
    class Window;
    class Display;
    class Clipboard;

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

    using MouseButtonFlags = std::uint32_t;
    namespace MouseButtonFlagBits
    {
        constexpr inline MouseButtonFlags LeftButton = 1 << 0;
        constexpr inline MouseButtonFlags MiddleButton = 1 << 0;
        constexpr inline MouseButtonFlags RightButton = 1 << 0;
        constexpr inline MouseButtonFlags X1Button = 1 << 0;
        constexpr inline MouseButtonFlags X2Button = 1 << 0;
    };

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

    using ModifierKeyFlags = std::uint32_t;
    namespace ModifierKeyFlagBits
    {
        constexpr inline ModifierKeyFlags LeftShift = 1 << 0;
        constexpr inline ModifierKeyFlags RightShift = 1 << 1;
        constexpr inline ModifierKeyFlags LeftControl = 1 << 2;
        constexpr inline ModifierKeyFlags RightControl = 1 << 3;
        constexpr inline ModifierKeyFlags LeftAlt = 1 << 4;
        constexpr inline ModifierKeyFlags RightAlt = 1 << 5;
        constexpr inline ModifierKeyFlags LeftGUI = 1 << 6; //Win
        constexpr inline ModifierKeyFlags RightGUI = 1 << 7;
    };

    struct VideoMode
    {
        std::uint32_t width;
        std::uint32_t height;
        std::uint32_t refresh_rate;
        std::uint32_t bits_per_pixel;
    };

    enum class WindowState : std::uint32_t
    {
        Windowed,
        FullScreen
    };

    enum class WindowVisibility : std::uint32_t
    {
        Hidden,
        Shown
    };

    enum class WindowDecorations : std::uint32_t
    {
        Disabled,
        Enabled
    };

    enum class WindowAlphaBlending : std::uint32_t
    {
        Disabled,
        Enabled
    };

    enum class WindowDragAndDropState : std::uint32_t
    {
        DeclineFiles,
        AcceptFiles
    };

    struct WindowInfo
    {
        WindowResolution resolution;
        WindowPosition position;
        WindowState state;
        WindowVisibility visibility;
        WindowDecorations decorations;
        WindowAlphaBlending alpha_blending;
        WindowDragAndDropState drag_and_drop_state;
        std::string_view title;
    };

    enum class CursorState : std::uint32_t
    {
        Enabled,
        Disbaled
    };

    enum class KeyboardAccessState : std::uint32_t
    {
        Exclusive,
        Shared
    };

#pragma message("Add other types like file list via hDrop")
    enum class ClipboardDataType : std::uint32_t
    {
        Unknown,
        MIME
    };

    using ClipboardCallback = void(Clipboard* clipboard);

    enum class WindowSubsystemConnectionType
    {
#ifdef _WIN32
        Win32 = 0
#endif
    };

    struct WindowSubsystemConnectionInfo
    {
        WindowSubsystemConnectionType type;
    };

    CORE_API std::vector<WindowSubsystemConnectionType>
    GetAvailableWindowSubsystemConnectionTypes();

    CORE_API WindowSubsystemConnection*
    CreateWindowSubsystemConnection(const WindowSubsystemConnectionInfo& info);

    CORE_API std::string KeyboardKeyToString(KeyboardKey key);
};
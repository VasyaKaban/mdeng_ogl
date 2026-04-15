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
    enum SpecialKey : KeyboardKey
    {
        KeyUnknown = 0,
        KeyEnter = 0xE0'00,
        KeyEscape,
        KeyBackSpace,
        KeyTab,
        KeySpace,
        KeyCapsLock,
        KeyF1,
        KeyF2,
        KeyF3,
        KeyF4,
        KeyF5,
        KeyF6,
        KeyF7,
        KeyF8,
        KeyF9,
        KeyF10,
        KeyF11,
        KeyF12,
        KeyPrintScreen,
        KeyScrollLock,
        KeyBreak,
        KeyPause,
        KeyInsert,
        KeyHome,
        KeyPageUp,
        KeyDelete,
        KeyEnd,
        KeyPageDown,
        KeyRightArrow,
        KeyLeftArrow,
        KeyDownArrow,
        KeyUpArrow,
        KeyNumLock,
        KeyKeypadDivide,
        KeyKeypadMultiply,
        KeyKeypadSubtract,
        KeyKeypadAdd,
        KeyKeypadEnter,
        KeyKeypad1,
        KeyKeypad2,
        KeyKeypad3,
        KeyKeypad4,
        KeyKeypad5,
        KeyKeypad6,
        KeyKeypad7,
        KeyKeypad8,
        KeyKeypad9,
        KeyKeypad0,
        KeyKeypadPeriod,
        KeyMenu,
        KeyKeyboardPower,
        KeyKeypadEqual,
        KeyF13,
        KeyF14,
        KeyF15,
        KeyF16,
        KeyF17,
        KeyF18,
        KeyF19,
        KeyF20,
        KeyF21,
        KeyF22,
        KeyF23,
        KeyF24,
        KeyBrazilKeypad,
        KeyRo, //Keyboard Int'l 1
        KeyKana, //Keyboard Intl'2
        KeyYen, //Keyboard Int'l 3
        KeyHenkan, //Keyboard Int'l 4
        KeyMuhenkan, //Keyboard Int'l 5
        KeyPC9800KeypadPeriod, //Keyboard Int'l 6
        KeyHanguel, //Keyboard Lang 1
        KeyHanja, //Keyboard Lang 2
        KeyKatakana, //Keyboard Lang 3
        KeyHiragana, //Keyboard Lang 4
        KeyZenkakuHankaku, //Keyboard Lang 5
        KeyLeftControl,
        KeyLeftShift,
        KeyLeftAlt,
        KeyLeftGUI,
        KeyRightControl,
        KeyRightShift,
        KeyRightAlt,
        KeyRightGUI,
        KeyScanNextTrack,
        KeyScanPreviousTrack,
        KeyStop,
        KeyPlayPause,
        KeyMute,
        KeyVolumeUp,
        KeyVolumeDown,
        KeyMediaSelect,
        KeyMail,
        KeyCalculator,
        KeyMyComputer,
        KeyWWWSearch,
        KeyWWWHome,
        KeyWWWBack,
        KeyWWWForward,
        KeyWWWStop,
        KeyWWWRefresh,
        KeyWWWFavorites
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
        LeftMeta = 1 << 6, //Win
        RightMeta = 1 << 7
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
};
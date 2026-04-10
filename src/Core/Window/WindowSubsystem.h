#pragma once

#include <vector>
#include "Core/API.h"
#include "Window.h"

namespace Core
{
    enum class WindowSubsystemType
    {
#ifdef _WIN32
        Win32 = 0
#elif defined(linux)
        XCB = 1
#endif
    };

    enum class CursorState
    {
        Enabled,
        Disbaled
    };

    struct KeyCodeDesc
    {
        char32_t utf32_char; //0 - unknown. also C0-C1 chars will be 0
        std::string_view name;
    };

    class CORE_API WindowSubsystem
    {
    public:
        virtual ~WindowSubsystem() = 0;

        virtual void PollEvents() = 0;

        virtual WindowSubsystemType GetType() const noexcept = 0;

        virtual Window* CreateWindow(const WindowInfo& info) = 0;

        virtual CursorState GetCursorState() const = 0;
        virtual void SetCursorState(CursorState state) = 0;

        virtual KeyCodeDesc GetKeyCodeDesc(RawKeyCode code) const noexcept = 0;
    };

    struct WindowSubsystemInfo
    {
        WindowSubsystemType type;
    };

    CORE_API std::vector<WindowSubsystemType> GetAvailableWindowSubsystemTypes();

    CORE_API WindowSubsystem* CreateWindowSubsystem(const WindowSubsystemInfo& info);
};
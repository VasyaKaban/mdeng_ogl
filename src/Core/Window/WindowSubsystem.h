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

    class CORE_API WindowSubsystem
    {
    public:
        virtual ~WindowSubsystem() = 0;

        virtual void PollEvents() = 0;

        virtual WindowSubsystemType GetType() const noexcept = 0;

        virtual Window* CreateWindow(const WindowInfo& info) = 0;

        virtual CursorState GetCursorState() const = 0;
        virtual void SetCursorState(CursorState state) = 0;
    };

    struct WindowSubsystemInfo
    {
        WindowSubsystemType type;
    };

    CORE_API std::vector<WindowSubsystemType> GetAvailableWindowSubsystemTypes();

    CORE_API WindowSubsystem* CreateWindowSubsystem(const WindowSubsystemInfo& info);
};
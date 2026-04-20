#pragma once

#include "Core/API.h"
#include "View.h"
#include "Core/Events/Events.h"

namespace Core
{
    class CORE_API WindowSubsystem : protected EventEmitter
    {
    public:
        using EventEmitter::Connect;

        virtual ~WindowSubsystem() = 0;

        virtual void PollEvents() = 0;

        virtual WindowSubsystemType GetType() const noexcept = 0;

        virtual Window* CreateWindow(const WindowInfo& info) = 0;

        virtual CursorState GetCursorState() const = 0;
        virtual void SetCursorState(CursorState state) = 0;

        virtual KeyboardKey GetKeyByScancode(ScanCode scancode) = 0;
        virtual std::optional<ScanCode> GetScanCodeFromKey(KeyboardKey key) = 0;
    };
};
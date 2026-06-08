#pragma once

#include <functional>
#include "Core/API.h"
#include "WindowSubsystem.h"
#include "WindowEvents.h"
#include "Core/Events/Events.h"

namespace Core
{
    class CORE_API WindowSubsystemConnection : protected ReservedEventEmitter<DisplayAddedEvent>
    {
    public:
        using EventEmitter::Connect;

        virtual ~WindowSubsystemConnection() = 0;

        virtual void PollEvents() = 0;

        virtual WindowSubsystemConnectionType GetType() const noexcept = 0;

        virtual Window* CreateWindow(const WindowInfo& info) = 0;

        virtual CursorState GetCursorState() const = 0;
        virtual void SetCursorState(CursorState state) = 0;

        virtual KeyboardKey GetKeyByScancode(ScanCode scancode) = 0;
        virtual std::optional<ScanCode> GetScanCodeFromKey(KeyboardKey key) = 0;

        virtual KeyboardAccessState GetKeyboardAccessState() = 0;
        virtual void SetKeyboardAccessState(KeyboardAccessState state) = 0;

        virtual void GetClipboard(const std::function<ClipboardCallback>& callback) = 0;

        virtual std::vector<RCPointer<Display>> GetDisplays() = 0;
    };
};
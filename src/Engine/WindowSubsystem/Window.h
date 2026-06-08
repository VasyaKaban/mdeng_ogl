#pragma once

#include <string_view>
#include <string>
#include "Core/API.h"
#include "WindowEvents.h"
#include "WindowSubsystem.h"
#include "Core/Events/Events.h"

namespace Core
{
    class CORE_API Window : protected ReservedEventEmitter<WindowClosedEvent,
                                                           WindowDisplayChangedEvent,
                                                           WindowMovedEvent,
                                                           WindowResizedEvent,
                                                           WindowMinimizedEvent,
                                                           WindowMaximizedEvent,
                                                           WindowHiddenEvent,
                                                           WindowShownEvent,
                                                           WindowCursorFocusGainEvent,
                                                           WindowCursorFocusLeaveEvent,
                                                           WindowKeyboardFocusGainEvent,
                                                           WindowKeyboardFocusLeaveEvent,
                                                           MouseButtonPressedEvent,
                                                           MouseButtonReleasedEvent,
                                                           MouseCursorMoveEvent,
                                                           MouseWheelEvent,
                                                           KeyboardKeyPressedEvent,
                                                           KeyboardCharacterPressedEvent,
                                                           KeyboardKeyReleasedEvent,
                                                           DragAndDropEvent>
    {
    public:
        using EventEmitter::Connect;

        virtual ~Window() = 0;

        virtual void Resize(const WindowResolution& resolution) = 0;
        virtual WindowResolution GetResolution() const = 0;

        virtual void Move(const WindowPosition& position) = 0;
        virtual WindowPosition GetPosition() const = 0;

        virtual void SetState(WindowState state) = 0;
        virtual WindowState GetState() const = 0;

        virtual void SetVisibility(WindowVisibility visibility) = 0;
        virtual WindowVisibility GetVisibility() const = 0;

        virtual void SetDecorations(WindowDecorations decorations) = 0;
        virtual WindowDecorations GetDecorations() const = 0;

        virtual void SetAlphaBlending(WindowAlphaBlending alpha_blending) = 0;
        virtual WindowAlphaBlending GetAlphaBlending() const = 0;

        virtual void SetDragAndDropState(WindowDragAndDropState drag_and_drop_state) = 0;
        virtual WindowDragAndDropState GetDragAndDropState() const = 0;

        virtual void SetTitle(std::string_view title) = 0;
        virtual std::string GetTitle() const = 0;

        virtual void SetMouseCursorPosition(const WindowPosition& pos) = 0; //relative to window
        virtual WindowPosition GetMouseCursorPosition() const = 0; //relative to window

        virtual RCPointer<Display> GetDisplay() const noexcept = 0;
        virtual WindowSubsystemConnection* GetParent() const noexcept = 0;
    };
};
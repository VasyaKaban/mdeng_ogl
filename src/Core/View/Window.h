#pragma once

#include <string_view>
#include <string>
#include "Core/API.h"
#include "View.h"
#include "Core/Events/Events.h"

namespace Core
{
    class CORE_API Window : protected EventEmitter
    {
    public:
        using EventEmitter::Connect;

        virtual ~Window() = 0;

        virtual void SetTitle(std::string_view title) = 0;
        virtual std::string GetTitle() const = 0;

        virtual void Resize(const WindowResolution& resolution) = 0;
        virtual WindowResolution GetResolution() const = 0;
        virtual WindowResolution GetScaledResolution() const = 0;

        virtual void SetState(WindowState state) = 0;
        virtual WindowState GetState() const = 0;

        virtual void SetMouseCursorPosition(const WindowPosition& pos) = 0; //relative to window
        virtual WindowPosition GetMouseCursorPosition() const = 0; //relative to window

        virtual void SetVisibility(WindowVisibility visibility) = 0;
        virtual WindowVisibility GetVisibility() const = 0;

        virtual WindowSurfaceInfo GetWindowSurfaceInfo() const noexcept = 0;

        virtual std::shared_ptr<Display> GetDisplay() const noexcept = 0;
        virtual WindowSubsystem* GetParent() const noexcept = 0;
    };
};
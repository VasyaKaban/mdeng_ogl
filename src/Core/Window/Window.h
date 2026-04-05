#pragma once

#include <string_view>
#include <string>
#include "Core/API.h"
#include "Core/Events/Events.h"
#include "WindowEvents.h"
#include "Core/Render/Render.h"

namespace Core
{
    enum class WindowState
    {
        Windowed = 0,
        FullScreen = 1
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

    class WindowSubsystem;
    class Display;

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

        virtual WindowSurfaceInfo GetWindowSurfaceInfo() const noexcept = 0;

        virtual Display* GetDisplay() const noexcept = 0;
        virtual WindowSubsystem* GetParent() const noexcept = 0;
    };
};
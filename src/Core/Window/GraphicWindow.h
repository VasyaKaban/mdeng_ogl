#pragma once

#include <SDL2/SDL.h>
#include "WindowEvents.h"
#include "Core/Events/Events.h"
#include "../Render/Render.h"

namespace Core
{
    struct GraphicWindowInfo
    {
        WindowResolution resolution;
        const char* title;
    };

    class WindowSubsystem;

    enum class WindowFullscreenState
    {
        Fullscreen = SDL_WINDOW_FULLSCREEN,
        Desktop = SDL_WINDOW_FULLSCREEN_DESKTOP,
        Windowed = 0
    };

    using WindowSurface = std::variant<Render::SurfaceWin32Info>;

    class CORE_API GraphicWindow : private Core::ReservedEventEmitter<WindowCloseEvent,
                                                                      WindowResizedEvent,
                                                                      WindowExposedEvent,
                                                                      WindowMovedEvent,
                                                                      MouseMotionEvent,
                                                                      MouseButtonEvent,
                                                                      MouseWheelEvent>
    {
        friend class WindowSubsystem;
        GraphicWindow(const GraphicWindowInfo& info);
    public:
        using Core::EventEmitter::Connect;

        ~GraphicWindow();

        void SetFullscreenState(WindowFullscreenState state);
        void SetTitle(const char* title);
        void SetResolution(const WindowResolution& resolution);

        SDL_Window* GetWindowHandle() const noexcept;

        WindowResolution GetResolution() const;

        std::uint32_t GetID() const noexcept;

        const WindowSurface& GetSurface() const noexcept;
    private:
        SDL_Window* handle;
        std::uint32_t id;

        WindowSurface surface;
    };
};
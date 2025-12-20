#pragma once

#include <memory>
#include <string_view>
#include <SDL2/SDL.h>
#include "hrs/non_creatable.hpp"
#include "WindowEvents.h"
#include "../Events/Events.hpp"
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

    enum class SurfaceType
    {
        Windows
    };

    class GraphicWindow : public Events::EventEmitter<WindowCloseEvent>,
                          public Events::EventEmitter<WindowResizedEvent>,
                          public Events::EventEmitter<WindowExposedEvent>,
                          public Events::EventEmitter<WindowMovedEvent>,
                          public Events::EventEmitter<MouseMotionEvent>,
                          public Events::EventEmitter<MouseButtonEvent>,
                          public Events::EventEmitter<MouseWheelEvent>
    {
        friend class WindowSubsystem;
        GraphicWindow(const GraphicWindowInfo& info);
    public:
        ~GraphicWindow();

        void SetFullscreenState(WindowFullscreenState state);
        void SetTitle(const char* title);
        void SetResolution(const WindowResolution& resolution);

        SDL_Window* GetWindowHandle() const noexcept;

        WindowResolution GetResolution() const;
        WindowResolution GetDrawableResolution() const;

        std::uint32_t GetID() const noexcept;

        SurfaceType GetSurfaceType() const noexcept;
        const Render::SurfaceWin32Info& GetWIN32SurfaceInfo() const;
    private:
        SDL_Window* handle;
        std::uint32_t id;

        SurfaceType surface_type;
        union
        {
            Render::SurfaceWin32Info win32;
        } surface_info;
    };
};
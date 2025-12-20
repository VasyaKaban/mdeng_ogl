#pragma once

#include <memory>
#include <string_view>
#include <SDL2/SDL.h>
#include "hrs/non_creatable.hpp"
#include "WindowEvents.h"
#include "../Events/Events.hpp"

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
    private:
        SDL_Window* handle;
        std::uint32_t id;
    };
};
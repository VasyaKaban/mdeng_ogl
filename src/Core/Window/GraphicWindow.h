#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include "hrs/non_creatable.hpp"
#include "EventHandlers.h"
#include "RenderBackend.h"

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

class GraphicWindow : hrs::non_copyable, hrs::non_movable
{
    friend class WindowSubsystem;
    GraphicWindow(const GraphicWindowInfo& info, const RenderBackendInfo& render_info);
public:
    ~GraphicWindow();

    void SetFullscreenState(WindowFullscreenState state);
    void SetTitle(const char* title);
    void SetResolution(const WindowResolution& resolution);

    SDL_Window* GetWindowHandle() const noexcept;

    WindowResolution GetResolution() const;
    WindowResolution GetDrawableResolution() const;

    EventHandlers& GetEventHandlers() noexcept;
    const EventHandlers& GetEventHandlers() const noexcept;

    RenderBackend* GetRenderBackend() const noexcept;

    std::uint32_t GetID() const noexcept;
private:
    SDL_Window* handle;
    std::uint32_t id;
    EventHandlers event_handlers;
    std::unique_ptr<RenderBackend> render_backend;
};
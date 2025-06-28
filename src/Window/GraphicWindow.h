#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include "../hrs/non_creatable.hpp"
#include "EventHandlers.h"

enum class OpenGLVersion;

#define MAKE_OPENGL_VERSION(MAJOR, MINOR, CORE) \
    MAJOR * 100 + MINOR * 10 + static_cast<int>(CORE == true)

enum class OpenGLVersion
{
    OpenGL_4_6_Core = MAKE_OPENGL_VERSION(4, 6, true)
};

#undef MAKE_OPENGL_VERSION

constexpr inline std::uint16_t GetOpenGLMajorVersion(OpenGLVersion version) noexcept
{
    return static_cast<std::uint16_t>(version) / 100;
}

constexpr inline std::uint16_t GetOpenGLMinorVersion(OpenGLVersion version) noexcept
{
    return (static_cast<std::uint16_t>(version) / 10) % 10;
}

constexpr inline bool IsOpenGLVersionCoreprofile(OpenGLVersion version) noexcept
{
    return static_cast<std::uint16_t>(version) % 10;
}

struct DefaultFramebufferInfo
{
    std::uint8_t red_channel_bits;
    std::uint8_t green_channel_bits;
    std::uint8_t blue_channel_bits;
    std::uint8_t alpha_channel_bits;

    std::uint8_t depth_channel_bits;
    std::uint8_t stencil_channel_bits;
};

struct GraphicWindowInfo
{
    DefaultFramebufferInfo default_framebuffer_info;
    OpenGLVersion opengl_version;
    std::uint16_t resource_set_count;

    std::uint32_t width;
    std::uint32_t height;

    const char* title;
};

class WindowSubsystem;
class Context;

enum class WindowFullscreenState
{
    Fullscreen = SDL_WINDOW_FULLSCREEN,
    Desktop = SDL_WINDOW_FULLSCREEN_DESKTOP,
    Windowed = 0
};

enum class WindowPresentMode
{
    Immediate = 0,
    VSync = 1,
    AdaptiveVSync = -1
};

class GraphicWindow : hrs::non_copyable, hrs::non_movable
{
    friend class WindowSubsystem;
    GraphicWindow(const GraphicWindowInfo& info);
public:
    ~GraphicWindow();

    void SetFullscreenState(WindowFullscreenState state);
    void SetTitle(const char* title) noexcept;
    void SetPresentMode(WindowPresentMode mode);

    SDL_Window* GetWindowHandle() noexcept;
    const SDL_Window* GetWindowHandle() const noexcept;

    EventHandlers& GetEventHandlers() noexcept;
    const EventHandlers& GetEvenetHandlers() const noexcept;

    void MakeContextCurrent() noexcept;
    void AcquireNextSwapchainImage(); //OGL -> noop
    void ReleaseSwapchainImage(); //SDL_SwapWindow();

    Context* GetContext() noexcept;
    const Context* GetContext() const noexcept;
private:
    SDL_Window* handle;
    std::uint32_t id;
    EventHandlers event_handlers;
    SDL_GLContext gl_ctx;
    std::unique_ptr<Context> ctx;
};
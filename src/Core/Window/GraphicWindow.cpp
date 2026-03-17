#include "GraphicWindow.h"
#include <stdexcept>
#include "Core/Utils/ScopedCall.hpp"
#include <SDL2/SDL_syswm.h>

namespace Core
{
    GraphicWindow::GraphicWindow(const GraphicWindowInfo& info)
    {
        SDL_Window* _handle = SDL_CreateWindow(info.title,
                                               SDL_WINDOWPOS_UNDEFINED,
                                               SDL_WINDOWPOS_UNDEFINED,
                                               info.resolution.width,
                                               info.resolution.height,
                                               SDL_WINDOW_RESIZABLE);

        if(!_handle)
            throw std::runtime_error(SDL_GetError());

        Core::ScopedCall cleanup = [_handle]()
        {
            SDL_DestroyWindow(_handle);
        };

        SDL_SysWMinfo sys_wm_info;
        SDL_VERSION(&sys_wm_info.version);

        auto res = SDL_GetWindowWMInfo(_handle, &sys_wm_info);
        if(res != SDL_TRUE)
            throw std::runtime_error(SDL_GetError());

#ifdef _WIN32
        if(sys_wm_info.subsystem != SDL_SYSWM_WINDOWS)
            throw std::runtime_error("Bad subsystem info");

        surface = Render::Win32SurfaceInfo{.window = sys_wm_info.info.win.window,
                                           .instance = sys_wm_info.info.win.hinstance};
#endif

        cleanup.Drop();

        handle = _handle;
        id = SDL_GetWindowID(_handle);
    }

    GraphicWindow::~GraphicWindow()
    {
        SDL_DestroyWindow(handle);
    }

    void GraphicWindow::SetFullscreenState(WindowFullscreenState state)
    {
        if(SDL_SetWindowFullscreen(handle, static_cast<Uint32>(state)) != 0)
            throw std::runtime_error(SDL_GetError());
    }

    void GraphicWindow::SetTitle(const char* title)
    {
        SDL_SetWindowTitle(handle, title);
    }

    void GraphicWindow::SetResolution(const WindowResolution& resolution)
    {
        SDL_SetWindowSize(handle, resolution.width, resolution.height);
    }

    SDL_Window* GraphicWindow::GetWindowHandle() const noexcept
    {
        return handle;
    }

    WindowResolution GraphicWindow::GetResolution() const
    {
        WindowResolution resolution;
        SDL_GetWindowSize(handle, &resolution.width, &resolution.height);

        return resolution;
    }

    std::uint32_t GraphicWindow::GetID() const noexcept
    {
        return id;
    }

    const WindowSurface& GraphicWindow::GetSurface() const noexcept
    {
        return surface;
    }
};
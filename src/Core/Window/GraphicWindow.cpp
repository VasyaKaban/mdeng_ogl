#include "GraphicWindow.h"
#include <stdexcept>
#include "hrs/scoped_call.hpp"
#include "OpenGL/OpenGLBackend.h"
#include <SDL2/SDL_syswm.h>

GraphicWindow::GraphicWindow(const GraphicWindowInfo& info, const RenderBackendInfo& render_info)
{
    if(render_info.type == RenderBackendType::OpenGL)
        OpenGLBackend::SetAttributes(static_cast<const OpenGLBackendInfo&>(render_info));

    SDL_Window* _handle = nullptr;

    hrs::scoped_call cleanup(
        [&_handle]()
        {
            if(_handle)
                SDL_DestroyWindow(_handle);
        });

    _handle = SDL_CreateWindow(info.title,
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               info.resolution.width,
                               info.resolution.height,
                               SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if(!_handle)
        throw std::runtime_error(SDL_GetError());

    this->handle = _handle;
    id = SDL_GetWindowID(_handle);

    if(render_info.type == RenderBackendType::OpenGL)
        render_backend.reset(
            new OpenGLBackend(this, static_cast<const OpenGLBackendInfo&>(render_info)));

    cleanup.drop();
}

GraphicWindow::~GraphicWindow()
{
    render_backend.reset();
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

WindowResolution GraphicWindow::GetDrawableResolution() const
{
    WindowResolution resolution;
    SDL_GL_GetDrawableSize(handle, &resolution.width, &resolution.height);

    return resolution;
}

RenderBackend* GraphicWindow::GetRenderBackend() const noexcept
{
    return render_backend.get();
}

std::uint32_t GraphicWindow::GetID() const noexcept
{
    return id;
}

std::string_view GraphicWindow::GetWindowManagerName() const
{
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version)

    if(SDL_GetWindowWMInfo(handle, &info) != SDL_TRUE)
        throw std::runtime_error(SDL_GetError());

    std::string_view name;
    switch(info.subsystem)
    {
        case SDL_SYSWM_UNKNOWN:
            name = "Unknown";
            break;
        case SDL_SYSWM_WINDOWS:
            name = "Windows";
            break;
        case SDL_SYSWM_X11:
            name = "X11";
            break;
        case SDL_SYSWM_DIRECTFB:
            name = "DirectFB";
            break;
        case SDL_SYSWM_COCOA:
            name = "Cocoa";
            break;
        case SDL_SYSWM_UIKIT:
            name = "UIKit";
            break;
        case SDL_SYSWM_WAYLAND:
            name = "Wayland";
            break;
        case SDL_SYSWM_MIR:
            name = "Mir";
            break;
        case SDL_SYSWM_WINRT:
            name = "WinRT";
            break;
        case SDL_SYSWM_ANDROID:
            name = "Android";
            break;
        case SDL_SYSWM_VIVANTE:
            name = "Vivante";
            break;
        case SDL_SYSWM_OS2:
            name = "OS/2";
            break;
        case SDL_SYSWM_HAIKU:
            name = "Unknown";
            break;
        case SDL_SYSWM_KMSDRM:
            name = "KMSDRM";
            break;
        case SDL_SYSWM_RISCOS:
            name = "RISCOS";
            break;
    }

    return name;
}
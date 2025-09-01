#include "GraphicWindow.h"
#include <stdexcept>
#include "hrs/scoped_call.hpp"
#include "OpenGL/OpenGLBackend.h"

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

    if(render_info.type == RenderBackendType::OpenGL)
        render_backend.reset(
            new OpenGLBackend(this, static_cast<const OpenGLBackendInfo&>(render_info)));

    cleanup.drop();

    handle = _handle;
    id = SDL_GetWindowID(_handle);
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

EventHandlers& GraphicWindow::GetEventHandlers() noexcept
{
    return event_handlers;
}

const EventHandlers& GraphicWindow::GetEventHandlers() const noexcept
{
    return event_handlers;
}

RenderBackend* GraphicWindow::GetRenderBackend() const noexcept
{
    return render_backend.get();
}

std::uint32_t GraphicWindow::GetID() const noexcept
{
    return id;
}
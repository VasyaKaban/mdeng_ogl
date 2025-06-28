#include "GraphicWindow.h"
#include <stdexcept>
#include "../hrs/scoped_call.hpp"
#include "../Render/Context/Context.h"

GraphicWindow::GraphicWindow(const GraphicWindowInfo& info)
{
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_DOUBLEBUFFER, true);

    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_RED_SIZE,
                        info.default_framebuffer_info.red_channel_bits);
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_GREEN_SIZE,
                        info.default_framebuffer_info.green_channel_bits);
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_BLUE_SIZE,
                        info.default_framebuffer_info.blue_channel_bits);
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_ALPHA_SIZE,
                        info.default_framebuffer_info.alpha_channel_bits);

    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_DEPTH_SIZE,
                        info.default_framebuffer_info.depth_channel_bits);
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_STENCIL_SIZE,
                        info.default_framebuffer_info.stencil_channel_bits);

    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MAJOR_VERSION,
                        GetOpenGLMajorVersion(info.opengl_version));
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MINOR_VERSION,
                        GetOpenGLMinorVersion(info.opengl_version));
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_PROFILE_MASK,
                        IsOpenGLVersionCoreprofile(info.opengl_version) ?
                            SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE :
                            SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    SDL_Window* _handle = nullptr;
    SDL_GLContext _gl_ctx = nullptr;
    Context* _ctx = nullptr;

    hrs::scoped_call cleanup(
        [&_handle, &_gl_ctx, &_ctx]()
        {
            if(_ctx)
                delete _ctx;

            if(_gl_ctx)
                SDL_GL_DeleteContext(_gl_ctx);

            if(_handle)
                SDL_DestroyWindow(_handle);
        });

    _handle = SDL_CreateWindow(info.title,
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               info.width,
                               info.height,
                               SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if(!_handle)
        throw std::runtime_error(SDL_GetError());

    _gl_ctx = SDL_GL_CreateContext(_handle);
    if(!_gl_ctx)
        throw std::runtime_error(SDL_GetError());

    SDL_GL_MakeCurrent(_handle, _gl_ctx);

    _ctx = new Context(this,
                       ContextInfo{.resource_set_count = info.resource_set_count},
                       reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress));

    cleanup.drop();

    handle = _handle;
    id = SDL_GetWindowID(_handle);
    gl_ctx = std::move(_gl_ctx);
    ctx.reset(_ctx);
}

GraphicWindow::~GraphicWindow()
{
    ctx.reset();
    SDL_GL_DeleteContext(gl_ctx);
    SDL_DestroyWindow(handle);
}

void GraphicWindow::SetFullscreenState(WindowFullscreenState state)
{
    if(SDL_SetWindowFullscreen(handle, static_cast<Uint32>(state)) != 0)
        throw std::runtime_error(SDL_GetError());
}

void GraphicWindow::SetTitle(const char* title) noexcept
{
    SDL_SetWindowTitle(handle, title);
}

void GraphicWindow::SetPresentMode(WindowPresentMode mode)
{
    if(SDL_GL_SetSwapInterval(static_cast<int>(mode)) != 0)
        throw std::runtime_error(SDL_GetError());
}

SDL_Window* GraphicWindow::GetWindowHandle() noexcept
{
    return handle;
}

const SDL_Window* GraphicWindow::GetWindowHandle() const noexcept
{
    return handle;
}

EventHandlers& GraphicWindow::GetEventHandlers() noexcept
{
    return event_handlers;
}

const EventHandlers& GraphicWindow::GetEvenetHandlers() const noexcept
{
    return event_handlers;
}

void GraphicWindow::MakeContextCurrent() noexcept
{
    SDL_GL_MakeCurrent(handle, gl_ctx);
}

void GraphicWindow::AcquireNextSwapchainImage() //OGL -> noop
{}

void GraphicWindow::ReleaseSwapchainImage() //SDL_SwapWindow();
{
    SDL_GL_SwapWindow(handle);
}

Context* GraphicWindow::GetContext() noexcept
{
    return ctx.get();
}

const Context* GraphicWindow::GetContext() const noexcept
{
    return ctx.get();
}
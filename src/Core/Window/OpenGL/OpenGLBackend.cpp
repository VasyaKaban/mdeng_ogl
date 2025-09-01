#include "OpenGLBackend.h"
#include "../GraphicWindow.h"
#include <stdexcept>

OpenGLBackend::OpenGLBackend(GraphicWindow* _parent, const OpenGLBackendInfo& info)
    : RenderBackend(_parent),
      version(info.version)
{
    handle = SDL_GL_CreateContext(parent->GetWindowHandle());
    if(!handle)
        throw std::runtime_error(SDL_GetError());

    SDL_GL_MakeCurrent(parent->GetWindowHandle(), handle);
}

void OpenGLBackend::SetAttributes(const OpenGLBackendInfo& info)
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
                        GetOpenGLMajorVersion(info.version));
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MINOR_VERSION,
                        GetOpenGLMinorVersion(info.version));
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_PROFILE_MASK,
                        IsOpenGLVersionCoreprofile(info.version) ?
                            SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE :
                            SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
}

OpenGLBackend::~OpenGLBackend()
{
    SDL_GL_DeleteContext(handle);
}

RenderBackendType OpenGLBackend::GetType() const noexcept
{
    return RenderBackendType::OpenGL;
}

void OpenGLBackend::SetPresentMode(WindowPresentMode mode)
{
    if(SDL_GL_SetSwapInterval(static_cast<int>(mode)) != 0)
        throw std::runtime_error(SDL_GetError());
}

void OpenGLBackend::MakeContextCurrent() noexcept
{
    SDL_GL_MakeCurrent(parent->GetWindowHandle(), handle);
}

void OpenGLBackend::AcquireNextSwapchainImage() //OGL -> noop
{
    //noop
}

void OpenGLBackend::ReleaseSwapchainImage() //SDL_SwapWindow();
{
    SDL_GL_SwapWindow(parent->GetWindowHandle());
}

OpenGLGetProcAddr_PFN OpenGLBackend::GetResolver() const noexcept
{
    return SDL_GL_GetProcAddress;
}

OpenGLVersion OpenGLBackend::GetVersion() const noexcept
{
    return version;
}
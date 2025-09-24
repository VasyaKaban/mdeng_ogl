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

    auto components = Render::GetFormatComponentsBitSize(info.default_framebuffer_info.format);
    if(components.red == 0 && components.green == 0 && components.blue == 0 &&
       components.alpha == 0)
        throw std::runtime_error("Bad format for OpenGL's default framebuffer");

    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_RED_SIZE, components.red);
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_GREEN_SIZE, components.green);
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_BLUE_SIZE, components.blue);
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_ALPHA_SIZE, components.alpha);

    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_STENCIL_SIZE, 0);

    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MAJOR_VERSION,
                        GetOpenGLMajorVersion(info.version));
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MINOR_VERSION,
                        GetOpenGLMinorVersion(info.version));
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_PROFILE_MASK,
                        IsOpenGLVersionCoreprofile(info.version) ?
                            SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE :
                            SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    if(info.flags & OpenGLBackendFlagBits::DebugContext)
        SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_FLAGS,
                            SDL_GLcontextFlag::SDL_GL_CONTEXT_DEBUG_FLAG);

#pragma warning("SDL_GL_FRAMEBUFFER_SRGB_CAPABLE check as color space!")
    //SDL_GL_CONTEXT_RELEASE_BEHAVIOR,    /**< sets context the release behavior; defaults to 1. (>= SDL 2.0.4) */
    //SDL_GL_CONTEXT_RESET_NOTIFICATION,
    //SDL_GL_CONTEXT_NO_ERROR,
    //SDL_GL_FLOATBUFFERS
    //SDL_GLcontextFlag::SDL_GL_CONTEXT_DEBUG_FLAG
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
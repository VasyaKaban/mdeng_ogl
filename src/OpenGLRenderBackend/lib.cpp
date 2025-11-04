#include "Context/Context.h"
#include <stdexcept>

void RenderInit()
{
    //noop
}

void RenderDestroy()
{
    //noop
}

Render::Context* RenderCreateContext(RenderBackend* backend,
                                     const Render::ContextSelector& selector)
{
    if(backend->GetType() != RenderBackendType::OpenGL)
        throw std::runtime_error("Uncompatible render backend type. Only OpenGL type supported");

    return new OpenGL::Context(static_cast<OpenGLBackend*>(backend), selector);
}

extern "C" Render::RenderResolve RenderResolve()
{
    return Render::RenderResolve{.init = RenderInit,
                                 .create_context = RenderCreateContext,
                                 .destroy = RenderDestroy};
}
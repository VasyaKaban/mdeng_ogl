#include "Resolve.h"
#include "OpenGLRenderBackend/API.h"

namespace OpenGL
{
    extern "C" OGL_RENDER_BACKEND_API_EXPORT Render::Resolve* RenderResolve()
    {
        return new Resolve;
    }
};
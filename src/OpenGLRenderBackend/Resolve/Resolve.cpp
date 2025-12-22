#include "Resolve.h"

namespace OpenGL
{
    extern "C" Render::Resolve* RenderResolve()
    {
        return new Resolve;
    }
};
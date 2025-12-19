#include "Resolve.h"
#include "../Objects/Instance/Instance.h"

namespace OpenGL
{
    extern "C" Render::Resolve* RenderResolve()
    {
        return new Resolve;
    }

    Resolve::Resolve()
    {}

    Resolve::~Resolve()
    {}

    void Resolve::Init()
    {}

    Render::Backend Resolve::GetBackend() const noexcept
    {
        return Render::Backend::OpenGL;
    }

    Render::Instance* CreateInstance(const Render::InstanceInfo& info)
    {
        return new Instance(info);
    }
};
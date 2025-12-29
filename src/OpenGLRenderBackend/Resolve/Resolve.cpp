#include "Resolve.h"
#include "../Objects/Instance/Instance.h"

namespace OpenGL
{
    extern "C" Render::Resolve* RenderResolve()
    {
        return new Resolve;
    }

    Resolve::Resolve()
        : instance_features{.validation_layer = true, .debug_messenger = true} //we use OGL4.5+
    {}

    Resolve::~Resolve()
    {}

    void Resolve::Init()
    {
        this->ResolveBase::Init();
    }

    const Render::InstanceFeatures& Resolve::GetInstanceFeatures() const noexcept
    {
        return instance_features;
    }

    Render::Backend Resolve::GetBackend() const noexcept
    {
        return Render::Backend::OpenGL;
    }

    Render::Instance* Resolve::CreateInstance(const Render::InstanceInfo& info)
    {
        return new Instance(info);
    }
};
#include "Resolve.h"
#include "Context.h"
#include <stdexcept>

namespace OpenGL
{
    static Context* context = nullptr;

    extern "C" Render::Resolve* RenderResolve()
    {
        static Resolve resolve;

        return &resolve;
    }

    Resolve::~Resolve()
    {}

    std::span<RenderBackendType> Resolve::GetAvailableBackends()
    {
        static RenderBackendType SUPPORTED_BACKEND = RenderBackendType::OpenGL;

        return {&SUPPORTED_BACKEND, 1};
    }

    void Resolve::Init(RenderBackend* backend)
    {
        if(backend->GetType() != RenderBackendType::OpenGL)
            throw std::runtime_error("Bad render backend type. 'OpenGL' type was expected");

        static bool created = false;
        if(created == true)
            throw std::runtime_error(
                "Implementation cannot create another context due to the internal implementation "
                "limitation");

        static Context ctx(static_cast<OpenGLBackend*>(backend));
        created = true;

        context = &ctx;
    }

    std::span<const Render::ContextProperties> Resolve::GetAvailableContexts()
    {
        static const Render::ContextProperties& props = context->GetProperties();

        return {&props, 1};
    }

    Render::Context* Resolve::CreateContext(const Render::SelectedContextDesc& desc)
    {
        if(desc.index != 0)
            throw std::runtime_error(
                "Bad selected context index. Implementation has only one available context");

        if(!(desc.queue_family_infos.size() == 1 && desc.queue_family_infos[0].index == 0 &&
             desc.queue_family_infos[0].queue_count == 1))
            throw std::runtime_error(
                "Bad selected context quyeue families. Implementation has only one queue");

        return context;
    }
};
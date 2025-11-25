#include "Resolve.h"
#include "Context.h"
#include <stdexcept>

namespace OpenGL
{
    extern "C" Render::Resolve* RenderResolve()
    {
        static Resolve resolve;

        return &resolve;
    }

    Resolve::~Resolve()
    {}

    std::span<const RenderBackendType> Resolve::GetAvailableBackends()
    {
        constexpr static RenderBackendType SUPPORTED_BACKEND = RenderBackendType::OpenGL;

        return {&SUPPORTED_BACKEND, 1};
    }

    void Resolve::Init(RenderBackend* backend)
    {
        if(ctx.get() != nullptr)
            throw std::runtime_error("Cannot init already inited resolve object");

        if(backend->GetType() != RenderBackendType::OpenGL)
            throw std::runtime_error("Bad render backend type. 'OpenGL' type was expected");

        ctx.reset(new Context(static_cast<OpenGLBackend*>(backend)));
    }

    std::span<const Render::ContextProperties> Resolve::GetAvailableContexts()
    {
        static const Render::ContextProperties& props = ctx->GetProperties();

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

        return ctx.get();
    }

    void Resolve::operator delete(void* ptr) noexcept
    {
        //noop -> static variable
    }
};
#include "RenderEngine.h"
#include "Core/Render/Context.h"
#include "Core/Render/Resolve.h"

namespace Engine
{
    RenderEngineState::RenderEngineState(const std::filesystem::path& implementation_path)
    {
        auto opt = lib.open(implementation_path);
        if(opt)
            throw opt.value();

        Render::PFN_RenderResolve resolve_pfn = reinterpret_cast<Render::PFN_RenderResolve>(
            lib.get_proc_address(Render::RENDER_RESOLVE_FUNCTION_NAME));
        if(!resolve_pfn)
            throw std::runtime_error(std::format("No {} in {} library found",
                                                 Render::RENDER_RESOLVE_FUNCTION_NAME,
                                                 implementation_path.string()));

        resolve = std::unique_ptr<Render::Resolve>(resolve_pfn());
    }

    void RenderEngineState::Init(Core::RenderBackend* backend)
    {
        resolve->Init(backend);
    }

    Render::Resolve* RenderEngineState::GetResolve() const noexcept
    {
        return resolve.get();
    }

    RenderEngine::RenderEngine(const RenderEngineInfo& info, RenderEngineState&& _state)
        : TaskRoot(nullptr, TaskKey{}),
          state(std::move(_state)),
          resource_set_count(info.resource_set_count),
          current_resource_set_index(0)
    {
        context.reset(state.GetResolve()->CreateContext(info.selected_context));

        Events::Connect<Core::WindowResizedEvent>(this, info.window, &RenderEngine::Handle);
    }

    RenderEngine::~RenderEngine()
    {
        context->WaitIdle();
    }

    std::uint16_t RenderEngine::GetResourceSetCount() const noexcept
    {
        return resource_set_count;
    }

    std::uint16_t RenderEngine::GetCurrentResourceSetIndex() const noexcept
    {
        return current_resource_set_index;
    }

    std::uint16_t RenderEngine::GetPreviousResourceSetIndex() const noexcept
    {
        if(current_resource_set_index == 0)
            return resource_set_count - 1;

        return (current_resource_set_index - 1) % resource_set_count;
    }

    std::uint16_t RenderEngine::GetNextResourceSetIndex() const noexcept
    {
        return (current_resource_set_index + 1) % resource_set_count;
    }

    void RenderEngine::AcquireNextResourceSet() noexcept
    {
        ClearDefferedEraseTasks();

        current_resource_set_index = (current_resource_set_index + 1) % resource_set_count;
    }

    EvaluateDesc RenderEngine::Begin(const EvaluateDesc& eval_desc)
    {
        return eval_desc;
    }

    void RenderEngine::End([[maybe_unused]] const EvaluateDesc& eval_desc)
    {
        //noop
    }

    Render::Context* RenderEngine::GetContext() const noexcept
    {
        return context.get();
    }

    Events::HandlerAction RenderEngine::Handle(const Core::WindowResizedEvent& event)
    {
        context->WaitIdle();

        return Events::HandlerAction::None;
    }
};
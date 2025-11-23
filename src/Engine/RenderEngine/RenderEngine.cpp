#include "RenderEngine.h"
#include "Core/Render/Context.h"

RenderEngine::RenderEngine(const RenderEngineInfo& info)
    : TaskRoot(nullptr, TaskKey{}),
      resolve_ctx(nullptr),
      resource_set_count(info.resource_set_count),
      current_resource_set_index(0)
{
    auto opt = lib.open(info.implementation);
    if(opt)
        throw opt.value();

    PFN_RenderResolve resolve_pfn =
        reinterpret_cast<PFN_RenderResolve>(lib.get_proc_address(RENDER_RESOLVE_FUNCTION_NAME));
    if(!resolve_pfn)
        throw std::runtime_error(std::format("No {} in {} library found",
                                             RENDER_RESOLVE_FUNCTION_NAME,
                                             info.implementation.string()));

    resolve_ctx = std::unique_ptr<RenderResolveContext>(resolve_pfn());
    context.reset(resolve_ctx->CreateContext(
        info.window->GetRenderBackend(),
        [&info](std::span<const Render::ContextProperties> properties) -> SelectedContextDesc
        {
            return SelectedContextDesc{.index = 0};
        }));

    Events::Connect<WindowResizedEvent>(this, info.window, &RenderEngine::Handle);
}

RenderEngine::~RenderEngine()
{
    context->WaitIdle();
    context.reset();

    resolve_ctx.reset();
    lib.close();
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

Events::HandlerAction RenderEngine::Handle(const WindowResizedEvent& event)
{
    context->WaitIdle();

    return Events::HandlerAction::None;
}
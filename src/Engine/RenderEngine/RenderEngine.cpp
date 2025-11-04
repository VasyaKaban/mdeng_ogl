#include "RenderEngine.h"
#include "Core/Render/Context.h"

#error RenderInterface to Engine header
#error GameInterface to Engine header
RenderEngine::RenderEngine(const RenderEngineInfo& info)
    : TaskRoot(nullptr, TaskKey{}),
      resolve({}),
      resource_set_count(info.resource_set_count),
      current_resource_set_index(0)
{
    auto opt = lib.open(info.implementation);
    if(opt)
        throw opt.value();

    Render::PFN_RenderResolve resolve_pfn = reinterpret_cast<Render::PFN_RenderResolve>(
        lib.get_proc_address(Render::RENDER_RESOLVE_FUNCTION_NAME));
    if(!resolve_pfn)
        throw std::runtime_error(std::format("No {} in {} library found",
                                             Render::RENDER_RESOLVE_FUNCTION_NAME,
                                             info.implementation.string()));

    resolve = resolve_pfn();
    context.reset(resolve.create_context(
        info.window->GetRenderBackend(),
        [&info](
            std::span<const Render::ContextProperties> properties) -> Render::SelectedContextDesc
        {
            return Render::SelectedContextDesc{.index = 0};
        }));

    Events::Connect<WindowResizedEvent>(this, info.window, &RenderEngine::Handle);
}

RenderEngine::~RenderEngine()
{
    context->WaitIdle();
    context.reset();

    if(resolve.destroy)
        resolve.destroy();

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
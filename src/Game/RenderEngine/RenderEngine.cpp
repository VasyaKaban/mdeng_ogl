#include "RenderEngine.h"
#include "Core/Render/Context.h"
#include "TransferQueue/TransferQueue.h"
#include "TransferQueue/TransferChannel.h"
#include "RenderQueue/RenderQueue.h"

RenderEngine::RenderEngine(const RenderEngineInfo& info,
                           std::unique_ptr<Render::Context>&& _context,
                           GraphicWindow* window)
    : TaskBase(nullptr, TaskKey{}),
      context(std::move(_context)),
      resource_set_count(info.resource_set_count),
      current_resource_set_index(0)
{
    Events::Connect<WindowResizedEvent>(this, window, &RenderEngine::Handle);

#pragma message("MOVE QUEUES OUTSIDE THE RENDER ENGINE!!! AS SCOPE OF STATE TRANSFER TASKS!!!")
    transfer_queue =
        new Task<TransferQueue>(static_cast<Task<RenderEngine>*>(this),
                                TaskKey{.priority = 0, .name = std::string_view("TransferQueue")});

    render_queue =
        new Task<RenderQueue>(static_cast<Task<RenderEngine>*>(this),
                              TaskKey{.priority = 1, .name = std::string_view("RenderQueue")});
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

Task<TransferQueue>* RenderEngine::GetTransferQueue() const noexcept
{
    return transfer_queue;
}

Task<RenderQueue>* RenderEngine::GetRenderQueue() const noexcept
{
    return render_queue;
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
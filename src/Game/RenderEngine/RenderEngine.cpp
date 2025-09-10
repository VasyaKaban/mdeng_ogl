#include "RenderEngine.h"
#include "Core/Render/Context.h"
#include "TransferQueue/TransferQueue.h"
#include "TransferQueue/TransferChannel.h"
#include "RenderQueue/RenderQueue.h"

RenderEngine::RenderEngine(const RenderEngineInfo& info,
                           std::unique_ptr<Render::Context>&& _context)
    : Task(nullptr, TaskKey{}),
      context(std::move(_context)),
      resource_set_count(info.resource_set_count),
      current_resource_set_index(0)
{
    auto transfer_queue_handle =
        std::unique_ptr<Render::Queue>(context->GetQueue(Render::QueueSpecialization::Transfer));
    transfer_queue = hrs::rc_ptr<TransferQueue>(
        new TransferQueue(this,
                          TaskKey{.priority = 0, .name = std::string_view("TransferQueue")},
                          std::move(transfer_queue_handle)));

    transfer_channel = hrs::rc_ptr<TransferChannel>(
        new TransferChannel(transfer_queue.get(),
                            TaskKey{.priority = 0, .name = std::string_view("TransferChannel")},
                            info.transfer_channel_info));

    auto render_queue_handle =
        std::unique_ptr<Render::Queue>(context->GetQueue(Render::QueueSpecialization::Graphics));
    render_queue = hrs::rc_ptr<RenderQueue>(
        new RenderQueue(this,
                        TaskKey{.priority = 1, .name = std::string_view("RenderQueue")},
                        std::move(render_queue_handle)));
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

TransferQueue* RenderEngine::GetTransferQueue() const noexcept
{
    return transfer_queue.get();
}

RenderQueue* RenderEngine::GetRenderQueue() const noexcept
{
    return render_queue.get();
}

TransferChannel* RenderEngine::GetTransferChannel() const noexcept
{
    return transfer_channel.get();
}

Render::Context* RenderEngine::GetContext() const noexcept
{
    return context.get();
}
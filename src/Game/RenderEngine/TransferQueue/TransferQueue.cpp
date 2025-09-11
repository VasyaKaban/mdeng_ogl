#include "TransferQueue.h"
#include "Core/Render/Context.h"
#include "../../RenderEngine/RenderEngine.h"

TransferQueue::TransferQueue(Task<RenderEngine>* _parent, TaskKey&& key)
    : QueueTask(_parent, std::move(key), Render::QueueSpecialization::Transfer),
      command_pool(_parent->GetContext()->CreateCommandPool(
          Render::CommandPoolInfo{.queue = QueueTask::handle.get()}))
{
    resources.reserve(_parent->GetResourceSetCount());
    for(std::size_t i = 0; i < _parent->GetResourceSetCount(); i++)
    {
        resources.push_back(Resource{
            .command_buffer = std::unique_ptr<Render::CommandBuffer>(command_pool->Allocate()),
            .fence = std::unique_ptr<Render::Fence>(_parent->GetContext()->CreateFence()),
            .signal_semaphore =
                std::unique_ptr<Render::Semaphore>(_parent->GetContext()->CreateSemaphore())});
    }
}

TransferQueue::~TransferQueue()
{
    for(auto& res: resources)
        res.fence->Wait(std::numeric_limits<std::uint64_t>::max());
}

EvaluateDesc TransferQueue::Begin([[maybe_unused]] const EvaluateDesc& eval_desc)
{
    auto& resource = resources[static_cast<RenderEngine*>(parent)->GetCurrentResourceSetIndex()];

    auto fence = resource.fence.get();
    if(!fence->Wait(std::numeric_limits<std::uint64_t>::max()))
        throw std::runtime_error("Failed to wait on TransferQueue fence");

    EvaluateDesc local_eval_desc =
        EvaluateDesc{.cmd = resource.command_buffer.get(), .pipeline = nullptr};
    Render::QueueBeginInfo begin_info = {.wait_seamphores = {}};

    handle->Begin(begin_info);
    local_eval_desc.cmd->Begin();

    return local_eval_desc;
}

void TransferQueue::End(const EvaluateDesc& eval_desc)
{
    auto& resource = resources[static_cast<RenderEngine*>(parent)->GetCurrentResourceSetIndex()];
    auto fence = resource.fence.get();

    eval_desc.cmd->End();

    Render::CommandBuffer* cmd = eval_desc.cmd;

    auto signal_sem = resource.signal_semaphore.get();
    Render::QueueFlushInfo flush_info = {.signal_fence = fence,
                                         .signal_seamphores = {&signal_sem, 1},
                                         .command_buffers = {&cmd, 1}};

    handle->Flush(flush_info);
}

Render::Semaphore* TransferQueue::GetCurrentSemaphore() const noexcept
{
    return resources[static_cast<RenderEngine*>(parent)->GetCurrentResourceSetIndex()]
        .signal_semaphore.get();
}

Render::Fence* TransferQueue::GetCurrentFence() const noexcept
{
    return resources[static_cast<RenderEngine*>(parent)->GetCurrentResourceSetIndex()].fence.get();
}

void TransferQueue::WaitAllFences()
{
    for(auto& res: resources)
    {
        if(!res.fence->Wait(std::numeric_limits<std::uint64_t>::max()))
            throw std::runtime_error("Failed to wait on TransferQueue fence");
    }
}
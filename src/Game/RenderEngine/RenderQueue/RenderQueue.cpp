#include "RenderQueue.h"
#include "../../RenderEngine/RenderEngine.h"
#include "../TransferQueue/TransferQueue.h"

RenderQueue::RenderQueue(RenderEngine* _parent, Queue&& _handle)
    : QueueTask(_parent, std::move(_handle)),
      TaskTreeHolder<RenderPassTask>(),
      command_pool(_parent->GetContext(), CommandPoolCreateInfo{.queue = &(QueueTask::handle)})
{
    resources.reserve(parent->GetResourceSetCount());
    for(std::size_t i = 0; i < parent->GetResourceSetCount(); i++)
    {
        resources.push_back(Resource{.command_buffer = command_pool.Allocate(),
                                     .fence = Fence(parent->GetContext()),
                                     .swapchain_wait_semaphore = Semaphore(parent->GetContext()),
                                     .signal_semaphore = Semaphore(parent->GetContext())});
    }
}

RenderQueue::~RenderQueue()
{
    for(auto& res: resources)
        res.fence.Wait(std::numeric_limits<std::uint64_t>::max());
}

void RenderQueue::Evaluate([[maybe_unused]] EvaluateDesc& eval_desc)
{
    auto& resource = resources[parent->GetCurrentResourceSetIndex()];

    auto fence = &resource.fence;
    if(!fence->Wait(std::numeric_limits<std::uint64_t>::max()))
        throw std::runtime_error("Failed to wait on RenderQueue fence");

    EvaluateDesc local_eval_desc = {.cmd = &resource.command_buffer, .pipeline = nullptr};
    auto wait_sem = parent->GetTransferQueue()->GetCurrentSemaphore();
    QueueBeginInfo begin_info = {.wait_seamphores = {&wait_sem, 1}};

    handle.Begin(begin_info);
    local_eval_desc.cmd->Begin();
    task_tree.Evaluate(local_eval_desc);
    local_eval_desc.cmd->End();

    auto signal_sem = &resource.signal_semaphore;
    QueueFlushInfo flush_info = {.signal_fence = fence,
                                 .signal_seamphores = {&signal_sem, 1},
                                 .command_buffers = {&local_eval_desc.cmd, 1}};
}

Semaphore* RenderQueue::GetCurrentSwapchainWaitSemaphore() noexcept
{
    return &resources[parent->GetCurrentResourceSetIndex()].swapchain_wait_semaphore;
}

Semaphore* RenderQueue::GetCurrentSignalSemaphore() noexcept
{
    return &resources[parent->GetCurrentResourceSetIndex()].signal_semaphore;
}

Fence* RenderQueue::GetCurrentFence() noexcept
{
    return &resources[parent->GetCurrentResourceSetIndex()].fence;
}

void RenderQueue::WaitAllFences()
{
    for(auto& res: resources)
    {
        if(!res.fence.Wait(std::numeric_limits<std::uint64_t>::max()))
            throw std::runtime_error("Failed to wait on RenderQueue fence");
    }
}
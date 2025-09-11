#include "RenderQueue.h"
#include "Core/Render/Context.h"
#include "../../RenderEngine/RenderEngine.h"
#include "../TransferQueue/TransferQueue.h"

RenderQueue::RenderQueue(Task<RenderEngine>* _parent, TaskKey&& key)
    : QueueTask(_parent, std::move(key), Render::QueueSpecialization::Graphics),
      command_pool(_parent->GetContext()->CreateCommandPool(
          Render::CommandPoolInfo{.queue = QueueTask::handle.get()}))
{
    resources.reserve(_parent->GetResourceSetCount());
    for(std::size_t i = 0; i < _parent->GetResourceSetCount(); i++)
    {
        resources.push_back(Resource{
            .command_buffer = std::unique_ptr<Render::CommandBuffer>(command_pool->Allocate()),
            .fence = std::unique_ptr<Render::Fence>(_parent->GetContext()->CreateFence()),
            .swapchain_wait_semaphore =
                std::unique_ptr<Render::Semaphore>(_parent->GetContext()->CreateSemaphore()),
            .signal_semaphore =
                std::unique_ptr<Render::Semaphore>(_parent->GetContext()->CreateSemaphore())});
    }
}

RenderQueue::~RenderQueue()
{
    for(auto& res: resources)
        res.fence->Wait(std::numeric_limits<std::uint64_t>::max());
}

EvaluateDesc RenderQueue::Begin([[maybe_unused]] const EvaluateDesc& eval_desc)
{
    auto& resource = resources[static_cast<RenderEngine*>(parent)->GetCurrentResourceSetIndex()];

    auto fence = resource.fence.get();
    if(!fence->Wait(std::numeric_limits<std::uint64_t>::max()))
        throw std::runtime_error("Failed to wait on RenderQueue fence");

    EvaluateDesc local_eval_desc = {.cmd = resource.command_buffer.get(), .pipeline = nullptr};
    std::array wait_sems = {
        static_cast<RenderEngine*>(parent)->GetTransferQueue()->GetCurrentSemaphore(),
        GetCurrentSwapchainWaitSemaphore()};

    Render::QueueBeginInfo begin_info = {.wait_seamphores = {wait_sems.data(), wait_sems.size()}};

    handle->Begin(begin_info);
    local_eval_desc.cmd->Begin();

    return local_eval_desc;
}

void RenderQueue::End(const EvaluateDesc& eval_desc)
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

Render::Semaphore* RenderQueue::GetCurrentSwapchainWaitSemaphore() noexcept
{
    return resources[static_cast<RenderEngine*>(parent)->GetCurrentResourceSetIndex()]
        .swapchain_wait_semaphore.get();
}

Render::Semaphore* RenderQueue::GetCurrentSignalSemaphore() noexcept
{
    return resources[static_cast<RenderEngine*>(parent)->GetCurrentResourceSetIndex()]
        .signal_semaphore.get();
}

Render::Fence* RenderQueue::GetCurrentFence() noexcept
{
    return resources[static_cast<RenderEngine*>(parent)->GetCurrentResourceSetIndex()].fence.get();
}

void RenderQueue::WaitAllFences()
{
    for(auto& res: resources)
    {
        if(!res.fence->Wait(std::numeric_limits<std::uint64_t>::max()))
            throw std::runtime_error("Failed to wait on RenderQueue fence");
    }
}
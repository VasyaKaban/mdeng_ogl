#include "TransferQueue.h"
#include "Core/Render/Context.h"
#include "../../RenderEngine/RenderEngine.h"

TransferQueue::State::State() noexcept
    : signal_fence(nullptr)
{}

void TransferQueue::State::Clear()
{
    wait_semaphores.clear();
    signal_semaphores.clear();
    signal_fence = nullptr;
}

TransferQueue::TransferQueue(RenderEngine* _parent, TaskKey&& key)
    : QueueTask(_parent,
                std::move(key),
                _parent->GetContext()->GetQueue(Render::QueueSpecializationFlagBits::TransferSpec)),
      command_pool(_parent->GetContext()->CreateCommandPoolUnique(
          Render::CommandPoolInfo{.queue = QueueTask::handle}))
{
    resources.reserve(_parent->GetResourceSetCount());
    for(std::size_t i = 0; i < _parent->GetResourceSetCount(); i++)
    {
        resources.push_back(Resource{.command_buffer = command_pool->AllocateUnique()});
    }
}

TransferQueue::~TransferQueue()
{}

EvaluateDesc TransferQueue::Begin([[maybe_unused]] const EvaluateDesc& eval_desc)
{
    auto& resource =
        resources[static_cast<RenderEngine*>(GetParent())->GetCurrentResourceSetIndex()];

    if(state.signal_fence && !state.signal_fence->Wait(std::numeric_limits<std::uint64_t>::max()))
        throw std::runtime_error("Failed to wait on TransferQueue fence");

    EvaluateDesc local_eval_desc =
        EvaluateDesc{.cmd = resource.command_buffer.get(), .pipeline = nullptr};
    Render::QueueBeginInfo begin_info = {.wait_seamphores = state.wait_semaphores};

    handle->Begin(begin_info);
    local_eval_desc.cmd->Begin();

    return local_eval_desc;
}

void TransferQueue::End(const EvaluateDesc& eval_desc)
{
    auto& resource =
        resources[static_cast<RenderEngine*>(GetParent())->GetCurrentResourceSetIndex()];

    eval_desc.cmd->End();

    Render::CommandBuffer* cmd = eval_desc.cmd;

    Render::QueueFlushInfo flush_info = {.signal_fence = state.signal_fence,
                                         .signal_seamphores = state.signal_semaphores,
                                         .command_buffers = {&cmd, 1}};

    handle->Flush(flush_info);

    state.Clear();
}

TransferQueue::State& TransferQueue::GetState() noexcept
{
    return state;
}
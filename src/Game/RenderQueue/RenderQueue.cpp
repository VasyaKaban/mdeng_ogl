#include "RenderQueue.h"
#include "Core/Render/Context.h"
#include "../../RenderEngine/RenderEngine.h"
#include "../TransferQueue/TransferQueue.h"

RenderQueue::State::State() noexcept
    : signal_fence(nullptr)
{}

void RenderQueue::State::Clear()
{
    signal_semaphores.clear();
    wait_semaphores.clear();
    signal_fence = nullptr;
}

RenderQueue::RenderQueue(RenderEngine* _parent, TaskKey&& key, Render::Queue* queue)
    : QueueTask(_parent, std::move(key), queue),
      command_pool(_parent->GetContext()->CreateCommandPoolUnique(
          Render::CommandPoolInfo{.queue = QueueTask::handle}))
{
    resources.reserve(_parent->GetResourceSetCount());
    for(std::size_t i = 0; i < _parent->GetResourceSetCount(); i++)
    {
        resources.push_back(Resource{.command_buffer = command_pool->AllocateUnique()});
    }
}

RenderQueue::~RenderQueue()
{}

EvaluateDesc RenderQueue::Begin([[maybe_unused]] const EvaluateDesc& eval_desc)
{
    auto& resource = resources[GetParent()->As<RenderEngine>()->GetCurrentResourceSetIndex()];

    if(state.signal_fence && !state.signal_fence->Wait(std::numeric_limits<std::uint64_t>::max()))
        throw std::runtime_error("Failed to wait on RenderQueue fence");

    EvaluateDesc local_eval_desc = {.cmd = resource.command_buffer.get(), .pipeline = nullptr};

    Render::QueueBeginInfo begin_info = {.wait_seamphores = state.wait_semaphores};

    handle->Begin(begin_info);
    local_eval_desc.cmd->Begin();

    return local_eval_desc;
}

void RenderQueue::End(const EvaluateDesc& eval_desc)
{
    auto& resource = resources[GetParent()->As<RenderEngine>()->GetCurrentResourceSetIndex()];

    eval_desc.cmd->End();

    Render::CommandBuffer* cmd = eval_desc.cmd;

    Render::QueueFlushInfo flush_info = {.signal_fence = state.signal_fence,
                                         .signal_seamphores = state.signal_semaphores,
                                         .command_buffers = {&cmd, 1}};

    handle->Flush(flush_info);

    state.Clear();
}

RenderQueue::State& RenderQueue::GetState() noexcept
{
    return state;
}
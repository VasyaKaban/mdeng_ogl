#pragma once

#include <vector>
#include "../Tasks/QueueTask.h"
#include "Core/Render/Objects/Queue.h"
#include "Core/Render/Objects/CommandBuffer.h"
#include "Core/Render/Objects/CommandPool.h"
#include "Core/Render/Objects/Fence.h"
#include "Core/Render/Objects/Semaphore.h"

class RenderEngine;
class TransferQueue;

class RenderQueue : public QueueTask
{
public:
    class State
    {
        friend class RenderQueue;
    public:
        State() noexcept;
        ~State() = default;

        //input
        std::vector<Render::Semaphore*> wait_semaphores;
        std::vector<Render::Semaphore*> signal_semaphores;
        Render::Fence* signal_fence;
    private:
        void Clear();
    };

    RenderQueue(RenderEngine* _parent, TaskKey&& key, Render::Queue* queue);
    virtual ~RenderQueue() override;

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) override;
    virtual void End(const EvaluateDesc& eval_desc) override;

    State& GetState() noexcept;
private:
    struct Resource
    {
        std::unique_ptr<Render::CommandBuffer> command_buffer;
    };

    std::unique_ptr<Render::CommandPool> command_pool;
    std::vector<Resource> resources;

    State state;
};

CHECK_TASK_IS_READY(RenderQueue)
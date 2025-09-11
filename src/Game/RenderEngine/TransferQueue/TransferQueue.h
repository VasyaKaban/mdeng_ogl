#pragma once

#include <vector>
#include "../Tasks/QueueTask.h"
#include "Core/Render/Objects/Queue.h"
#include "Core/Render/Objects/Fence.h"
#include "Core/Render/Objects/Semaphore.h"
#include "Core/Render/Objects/CommandPool.h"
#include "Core/Render/Objects/CommandBuffer.h"

class RenderEngine;

class TransferQueue : public QueueTask
{
public:
    TransferQueue(Task<RenderEngine>* _parent, TaskKey&& key);
    virtual ~TransferQueue() override;

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) override;
    virtual void End(const EvaluateDesc& eval_desc) override;

    Render::Semaphore* GetCurrentSemaphore() const noexcept;
    Render::Fence* GetCurrentFence() const noexcept;
    void WaitAllFences();
private:
    struct Resource
    {
        std::unique_ptr<Render::CommandBuffer> command_buffer;
        std::unique_ptr<Render::Fence> fence;
        std::unique_ptr<Render::Semaphore> signal_semaphore;
    };

    std::unique_ptr<Render::CommandPool> command_pool;
    std::vector<Resource> resources;
};

CHECK_TASK_IS_READY(TransferQueue)
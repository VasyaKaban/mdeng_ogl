#pragma once

#include <vector>
#include "../Tasks/QueueTask.h"
#include "Core/Render/Objects/Queue.h"
#include "Core/Render/Objects/CommandBuffer.h"
#include "Core/Render/Objects/CommandPool.h"
#include "Core/Render/Objects/Fence.h"
#include "Core/Render/Objects/Semaphore.h"

class RenderEngine;

class RenderQueue : public QueueTask
{
public:
    RenderQueue(Task<RenderEngine>* _parent, TaskKey&& key);
    virtual ~RenderQueue() override;

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) override;
    virtual void End(const EvaluateDesc& eval_desc) override;

    Render::Semaphore* GetCurrentSwapchainWaitSemaphore() noexcept;
    Render::Semaphore* GetCurrentSignalSemaphore() noexcept;
    Render::Fence* GetCurrentFence() noexcept;
    void WaitAllFences();
private:
    struct Resource
    {
        std::unique_ptr<Render::CommandBuffer> command_buffer;
        std::unique_ptr<Render::Fence> fence;
        std::unique_ptr<Render::Semaphore> swapchain_wait_semaphore;
        std::unique_ptr<Render::Semaphore> signal_semaphore;
    };

    std::unique_ptr<Render::CommandPool> command_pool;
    std::vector<Resource> resources;
};

CHECK_TASK_IS_READY(RenderQueue)
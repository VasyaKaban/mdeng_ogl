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
    RenderQueue(RenderEngine* _parent, TaskKey&& key, std::unique_ptr<Render::Queue>&& _handle);
    virtual ~RenderQueue() override;

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) override;
    virtual void End(const EvaluateDesc& eval_desc) override;

    virtual void Enable() override;
    virtual void Disable() override;

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

static_assert(!std::is_abstract_v<RenderQueue>);
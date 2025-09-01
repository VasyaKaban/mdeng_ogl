#pragma once

#include <vector>
#include "hrs/non_creatable.hpp"
#include "../Tasks/QueueTask.h"
#include "../Tasks/RenderPassTask.h"
#include "Core/Render/Objects/Queue/Queue.h"
#include "Core/Render/Objects/CommandBuffer/CommandBuffer.h"
#include "Core/Render/Objects/CommandPool/CommandPool.h"
#include "Core/Render/Objects/Fence/Fence.h"
#include "Core/Render/Objects/Semaphore/Semaphore.h"
#include "../TaskTree/TaskTree.h"
#include "../TaskTree/TaskTreeHolder.hpp"

class RenderEngine;

class RenderQueue : hrs::non_movable, public QueueTask, public TaskTreeHolder<RenderPassTask>
{
public:
    RenderQueue(RenderEngine* _parent, Queue&& _handle);
    virtual ~RenderQueue() override;

    virtual void Evaluate(EvaluateDesc& eval_desc) override;

    Semaphore* GetCurrentSwapchainWaitSemaphore() noexcept;
    Semaphore* GetCurrentSignalSemaphore() noexcept;
    Fence* GetCurrentFence() noexcept;
    void WaitAllFences();
private:
    struct Resource
    {
        CommandBuffer command_buffer;
        Fence fence;
        Semaphore swapchain_wait_semaphore;
        Semaphore signal_semaphore;
    };

    CommandPool command_pool;
    std::vector<Resource> resources;
};

static_assert(!std::is_abstract_v<RenderQueue>);
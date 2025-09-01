#pragma once

#include <vector>
#include "hrs/non_creatable.hpp"
#include "../Tasks/QueueTask.h"
#include "Core/Render/Objects/Queue/Queue.h"
#include "../TaskTree/TaskTree.h"
#include "../TaskTree/TaskTreeHolder.hpp"
#include "Core/Render/Objects/Fence/Fence.h"
#include "Core/Render/Objects/Semaphore/Semaphore.h"
#include "Core/Render/Objects/CommandPool/CommandPool.h"
#include "Core/Render/Objects/CommandBuffer/CommandBuffer.h"

class RenderEngine;

class TransferQueue : hrs::non_movable, public QueueTask, public TaskTreeHolder<Task>
{
public:
    TransferQueue(RenderEngine* _parent, Queue&& _handle);
    virtual ~TransferQueue() override;

    virtual void Evaluate(EvaluateDesc& eval_desc) override;

    Semaphore* GetCurrentSemaphore() noexcept;
    Fence* GetCurrentFence() noexcept;
    void WaitAllFences();
private:
    struct Resource
    {
        CommandBuffer command_buffer;
        Fence fence;
        Semaphore signal_semaphore;
    };

    CommandPool command_pool;
    std::vector<Resource> resources;
};

static_assert(!std::is_abstract_v<TransferQueue>);
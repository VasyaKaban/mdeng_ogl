#pragma once

#include <vector>
#include "../Tasks/QueueTask.h"
#include "Core/Render/Objects/Queue.h"
#include "Core/Render/Objects/Fence.h"
#include "Core/Render/Objects/Semaphore.h"
#include "Core/Render/Objects/CommandPool.h"
#include "Core/Render/Objects/CommandBuffer.h"
#include "TransferOperation.h"

class RenderEngine;

class TransferStorage : hrs::non_copyable, hrs::non_movable
{
public:
    TransferStorage(RenderEngine* _parent);
    ~TransferStorage();

    void Transfer(TransferBufferOperation&& op);
    void Transfer(TransferImageOperation&& op);
    void Transfer(TransferCallbackOperation&& op);

    void Reserve(std::size_t size);

    void Flush();
private:
    void start_write();
private:
    RenderEngine* parent;
    Render::Queue* queue;
    std::unique_ptr<Render::CommandPool> command_pool;
    std::unique_ptr<Render::CommandBuffer> command_buffer;
    bool write_started;
};
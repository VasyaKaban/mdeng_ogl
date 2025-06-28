#pragma once

#include <span>
#include "../../../hrs/non_creatable.hpp"
#include "../../Common.h"

class Context;
class Fence;
class Queue;
class Semaphore;

struct QueueBeginInfo
{
    std::span<Semaphore*> wait_seamphores;
};

struct QueueFlushInfo
{
    Fence* signal_fence;
    std::span<Semaphore*> signal_seamphores;
};

class Queue : hrs::non_copyable
{
public:
    Queue() noexcept;
    Queue(Context* _parent) noexcept;
    ~Queue();
    Queue(Queue&& q) noexcept;
    Queue& operator=(Queue&& q) noexcept;

    void Begin(QueueBeginInfo& info);
    void Flush(QueueFlushInfo& info);

    bool IsCreated() const noexcept;
private:
    void destroy() noexcept;
private:
    Context* parent;
};
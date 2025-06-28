#include "Queue.h"
#include "../../Context/Context.h"
#include "../Fence/Fence.h"
#include "../Semaphore/Semaphore.h"

Queue::Queue() noexcept
    : parent(nullptr)
{}

Queue::Queue(Context* _parent) noexcept
    : parent(_parent)
{}

Queue::~Queue()
{
    destroy();
}

Queue::Queue(Queue&& q) noexcept
    : parent(q.parent)
{}

Queue& Queue::operator=(Queue&& q) noexcept
{
    destroy();

    parent = q.parent;

    return *this;
}

void Queue::Begin(QueueBeginInfo& info)
{
    for(auto& sem: info.wait_seamphores)
        sem->Wait();
}

void Queue::Flush(QueueFlushInfo& info)
{
    parent->GetLoader().MemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);

    for(auto& sem: info.signal_seamphores)
        sem->Set();

    if(info.signal_fence)
        info.signal_fence->Set();

    parent->GetLoader().Flush();
}

bool Queue::IsCreated() const noexcept
{
    return parent != nullptr;
}

void Queue::destroy() noexcept
{}
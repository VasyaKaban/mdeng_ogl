#include "Queue.h"
#include "../../Context/Context.h"
#include "../Fence/Fence.h"
#include "../Semaphore/Semaphore.h"

namespace OpenGL
{
    Queue::Queue(Context* _parent) noexcept
        : parent(_parent)
    {}

    Queue::~Queue()
    {
        //noop
    }

    void Queue::Begin(Render::QueueBeginInfo& info)
    {
        for(auto& sem: info.wait_seamphores)
            static_cast<Semaphore*>(sem)->Wait();
    }

    void Queue::Flush(Render::QueueFlushInfo& info)
    {
        parent->GetLoader().MemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);

        for(auto& sem: info.signal_seamphores)
            static_cast<Semaphore*>(sem)->Set();

        if(info.signal_fence)
            static_cast<Fence*>(info.signal_fence)->Set();

        parent->GetLoader().Flush();
    }

    Render::Context* Queue::GetContext() const noexcept
    {
        return parent;
    }
};
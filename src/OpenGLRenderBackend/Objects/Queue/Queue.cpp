#include "Queue.h"
#include "../Device/Device.h"
#include "../Fence/Fence.h"
#include "../Semaphore/Semaphore.h"

namespace OpenGL
{
    Queue::Queue(Device* _parent) noexcept
        : parent(_parent)
    {}

    Queue::~Queue()
    {
        //noop
    }

    void Queue::Begin(const Render::QueueBeginInfo& info)
    {
        for(auto& desc: info.wait_descs)
            static_cast<Semaphore*>(desc.semaphore)->Wait();
    }

    void Queue::Flush(const Render::QueueFlushInfo& info)
    {
        //We do not need a memory barrier here because in opengl barriers works as dependency signal between shader(!) and next commands or host reads/writes
        //parent->GetLoader().MemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);

        for(auto& sem: info.signal_seamphores)
            static_cast<Semaphore*>(sem)->Set();

        if(info.signal_fence)
            static_cast<Fence*>(info.signal_fence)->Set();

        parent->GetLoader().Flush();
    }

    void Queue::WaitIdle()
    {
        parent->GetLoader().Finish();
    }

    Render::Device* Queue::GetParent() const noexcept
    {
        return parent;
    }
};
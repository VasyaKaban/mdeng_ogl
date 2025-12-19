#include "Semaphore.h"
#include <stdexcept>
#include "../Device/Device.h"

namespace OpenGL
{
    void Semaphore::Wait()
    {
#ifndef OPENGL_NOOP_SEMAPHORE
        if(!handle)
            return; //skip waiting on non-created semaphore like in Fence class

        parent->GetLoader().WaitSync(handle, 0, GL_TIMEOUT_IGNORED);
#endif
    }

    void Semaphore::Set()
    {
#ifndef OPENGL_NOOP_SEMAPHORE
        if(handle)
            parent->GetLoader().DeleteSync(handle);

        handle = parent->GetLoader().FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        if(!handle)
            throw std::runtime_error("Failed to create semaphore  object");
#endif
    }

    Semaphore::Semaphore(Device* _parent)
        : parent(_parent)
#ifndef OPENGL_NOOP_SEMAPHORE
          ,
          handle(nullptr)
#endif
    {}

    Semaphore::~Semaphore()
    {
#ifndef OPENGL_NOOP_SEMAPHORE
        parent->GetLoader().DeleteSync(handle);
#endif
    }

    Render::Device* Semaphore::GetParent() const noexcept
    {
        return parent;
    }

    GLsync Semaphore::GetHandle() const noexcept
    {
#ifndef OPENGL_NOOP_SEMAPHORE
        return handle;
#else
        return nullptr;
#endif
    }
};
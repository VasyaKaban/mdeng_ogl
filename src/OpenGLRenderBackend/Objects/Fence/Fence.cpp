#pragma once

#include "Fence.h"
#include "../../Context/Context.h"
#include <stdexcept>

namespace OpenGL
{
    void Fence::Set()
    {
        if(handle)
            throw std::runtime_error("Fence is in signaled state");

        handle = parent->GetLoader().FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        if(!handle)
            throw std::runtime_error("Failed to create fence object");
    }

    Fence::Fence(Context* _parent) noexcept
        : parent(_parent),
          handle(nullptr)
    {}

    Fence::~Fence()
    {
        Wait(0);
        parent->GetLoader().DeleteSync(handle);
    }

    bool Fence::Wait(std::uint64_t timeout_ns) noexcept
    {}

    Render::FenceStatus Fence::GetStatus() const noexcept
    {
        if(!handle)
            return Render::FenceStatus::Signaled;

        GLint status;
        parent->GetLoader().GetSynciv(handle, GL_SYNC_STATUS, sizeof(status), nullptr, &status);

        return (status == GL_SIGNALED ? Render::FenceStatus::Signaled :
                                        Render::FenceStatus::Unsignaled);
    }

    GLsync Fence::GetHandle() const noexcept
    {
        return handle;
    }

    Render::Context* Fence::GetContext() const noexcept
    {
        return parent;
    }
};
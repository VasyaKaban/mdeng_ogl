#pragma once

#include "Fence.h"
#include "../Device/Device.h"
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

    Fence::Fence(Device* _parent) noexcept
        : parent(_parent),
          handle(nullptr)
    {}

    Fence::~Fence()
    {
        Wait(0);
        parent->GetLoader().DeleteSync(handle);
    }

    bool Fence::Wait(std::uint64_t timeout_ns) noexcept
    {
        if(!handle)
            return true;

        auto res = parent->GetLoader().ClientWaitSync(handle, 0, timeout_ns);
        if(res == GL_CONDITION_SATISFIED || res == GL_ALREADY_SIGNALED)
        {
            parent->GetLoader().DeleteSync(handle);
            handle = nullptr;
            return true;
        }

        return false;
    }

    Render::FenceStatus Fence::GetStatus() const noexcept
    {
        if(!handle)
            return Render::FenceStatus::Signaled;

        GLint status;
        parent->GetLoader().GetSynciv(handle, GL_SYNC_STATUS, sizeof(status), nullptr, &status);

        return (status == GL_SIGNALED ? Render::FenceStatus::Signaled :
                                        Render::FenceStatus::Unsignaled);
    }

    Render::Device* Fence::GetParent() const noexcept
    {
        return parent;
    }

    GLsync Fence::GetHandle() const noexcept
    {
        return handle;
    }
};
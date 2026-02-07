#pragma once

#include "Fence.h"
#include "../Device/Device.h"
#include <stdexcept>

namespace OpenGL
{
    //handle = nullptr -> unsignaled
    void Fence::Set()
    {
        if(handle)
            throw std::runtime_error("Fence is already set");

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
        return (res == GL_CONDITION_SATISFIED || res == GL_ALREADY_SIGNALED);
    }

    bool Fence::Reset() noexcept
    {
        if(!handle)
            return true;

        parent->GetLoader().DeleteSync(handle);
        handle = nullptr;

        return true;
    }

    Render::FenceStatus Fence::GetStatus() const noexcept
    {
        if(!handle)
            return Render::FenceStatus::Unsignaled;

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
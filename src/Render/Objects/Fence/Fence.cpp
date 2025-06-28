#pragma once

#include "Fence.h"
#include "../../Context/Context.h"
#include <stdexcept>

void Fence::Set()
{
    if(handle)
        throw std::runtime_error("Fence is in signaled state");

    handle = parent->GetLoader().FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    if(!handle)
        throw std::runtime_error("Failed to create fence object");
}

Fence::Fence(Context* _parent)
    : parent(_parent),
      handle(nullptr)
{}

Fence::~Fence()
{
    destroy();
}

Fence::Fence(Fence&& f) noexcept
    : parent(f.parent),
      handle(std::exchange(f.handle, nullptr))
{}

Fence& Fence::operator=(Fence&& f) noexcept
{
    destroy();

    parent = f.parent;
    handle = std::exchange(f.handle, nullptr);

    return *this;
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

bool Fence::IsCreated() const noexcept
{
    return parent != nullptr;
}

void Fence::destroy()
{
    Wait(0);
    parent->GetLoader().DeleteSync(handle);
}
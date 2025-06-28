#pragma once

#include "../../Common.h"
#include "../../../hrs/non_creatable.hpp"

class Context;

class Fence : hrs::non_copyable
{
    friend class Queue;
    void Set();
public:
    Fence(Context* _parent);
    ~Fence();
    Fence(Fence&& f) noexcept;
    Fence& operator=(Fence&& f) noexcept;

    bool Wait(std::uint64_t timeout_ns) noexcept;

    bool IsCreated() const noexcept;
private:
    void destroy();
private:
    Context* parent;
    GLsync handle;
};
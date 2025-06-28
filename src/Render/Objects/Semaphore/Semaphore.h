#pragma once

#include "../../../hrs/non_creatable.hpp"
#include "../../Common.h"

#define NOOP_SEMAPHORE

class Context;
class Queue;

class Semaphore : hrs::non_copyable
{
    friend class Queue;
    friend class Context;
    void Wait();
    void Set();
public:
    Semaphore() noexcept;
    Semaphore(Context* _parent);
    ~Semaphore();
    Semaphore(Semaphore&& sem) noexcept;
    Semaphore& operator=(Semaphore&& sem) noexcept;

    bool IsCreated() const noexcept;
private:
    void destroy() noexcept;
private:
    Context* parent;
    GLsync handle;
};
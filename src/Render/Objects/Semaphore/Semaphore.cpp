#include "Semaphore.h"
#include <stdexcept>
#include "../../Context/Context.h"

void Semaphore::Wait()
{
#ifndef NOOP_SEMAPHORE
    if(!handle)
        throw std::runtime_error("Wait on unset semaphore");

    parent->GetLoader().WaitSync(handle, 0, GL_TIMEOUT_IGNORED);
#endif
}

void Semaphore::Set()
{
#ifndef NOOP_SEMAPHORE
    if(handle)
        parent->GetLoader().DeleteSync(handle);

    handle = parent->GetLoader().FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    if(!handle)
        throw std::runtime_error("Failed to create semaphore  object");
#endif
}

Semaphore::Semaphore() noexcept
    : parent(nullptr),
      handle(nullptr)
{}

Semaphore::Semaphore(Context* _parent)
    : parent(_parent),
      handle(nullptr)
{}

Semaphore::~Semaphore()
{
    destroy();
}

Semaphore::Semaphore(Semaphore&& sem) noexcept
    : parent(sem.parent),
      handle(std::exchange(sem.handle, nullptr))
{}

Semaphore& Semaphore::operator=(Semaphore&& sem) noexcept
{
    destroy();

    parent = sem.parent;
    handle = std::exchange(sem.handle, nullptr);

    return *this;
}

bool Semaphore::IsCreated() const noexcept
{
    return parent != nullptr;
}

void Semaphore::destroy() noexcept
{
#ifndef NOOP_SEMAPHORE
    parent->GetLoader().DeleteSync(handle);
#endif
}

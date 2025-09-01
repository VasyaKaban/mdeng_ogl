#include "QueueTask.h"
#include "../RenderEngine.h"

QueueTask::QueueTask(RenderEngine* _parent, Queue&& _handle) noexcept
    : parent(_parent),
      handle(std::move(_handle))
{}

Task* QueueTask::GetParent() noexcept
{
    return parent;
}

const Task* QueueTask::GetParent() const noexcept
{
    return parent;
}

Queue& QueueTask::GetHandle() noexcept
{
    return handle;
}

const Queue& QueueTask::GetHandle() const noexcept
{
    return handle;
}

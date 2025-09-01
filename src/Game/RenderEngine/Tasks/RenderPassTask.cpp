#include "RenderPassTask.h"
#include "../RenderQueue/RenderQueue.h"

RenderPassTask::RenderPassTask(RenderQueue* _parent, RenderPass&& _handle) noexcept
    : parent(_parent),
      handle(std::move(_handle))
{}

Task* RenderPassTask::GetParent() noexcept
{
    return parent;
}

const Task* RenderPassTask::GetParent() const noexcept
{
    return parent;
}

RenderPass& RenderPassTask::GetHandle() noexcept
{
    return handle;
}

const RenderPass& RenderPassTask::GetHandle() const noexcept
{
    return handle;
}
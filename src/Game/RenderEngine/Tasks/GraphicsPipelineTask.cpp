#include "GraphicsPipelineTask.h"
#include "RenderPassTask.h"

GraphicsPipelineTask::GraphicsPipelineTask(RenderPassTask* _parent, Pipeline&& _handle) noexcept
    : parent(_parent),
      handle(std::move(_handle))
{}

Task* GraphicsPipelineTask::GetParent() noexcept
{
    return parent;
}

const Task* GraphicsPipelineTask::GetParent() const noexcept
{
    return parent;
}

Pipeline& GraphicsPipelineTask::GetHandle() noexcept
{
    return handle;
}

const Pipeline& GraphicsPipelineTask::GetHandle() const noexcept
{
    return handle;
}

#include "MaterialTask.h"
#include "GraphicsPipelineTask.h"

MaterialTask::MaterialTask(GraphicsPipelineTask* _parent) noexcept
    : parent(_parent)
{}

Task* MaterialTask::GetParent() noexcept
{
    return parent;
}

const Task* MaterialTask::GetParent() const noexcept
{
    return parent;
}

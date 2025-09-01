#include "InstanceTask.h"
#include "MeshTask.h"

InstanceTask::InstanceTask(MeshTask* _parent) noexcept
    : parent(_parent)
{}

Task* InstanceTask::GetParent() noexcept
{
    return parent;
}

const Task* InstanceTask::GetParent() const noexcept
{
    return parent;
}

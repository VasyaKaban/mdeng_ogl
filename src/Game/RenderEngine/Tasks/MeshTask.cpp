#include "MeshTask.h"
#include "MaterialTask.h"

MeshTask::MeshTask(MaterialTask* _parent) noexcept
    : parent(_parent)
{}

Task* MeshTask::GetParent() noexcept
{
    return parent;
}

const Task* MeshTask::GetParent() const noexcept
{
    return parent;
}

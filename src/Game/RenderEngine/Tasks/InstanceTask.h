#pragma once

#include "hrs/non_creatable.hpp"
#include "../TaskTree/Task.h"

class MeshTask;

class InstanceTask : hrs::non_movable, public Task
{
public:
    InstanceTask(MeshTask* _parent) noexcept;
    virtual ~InstanceTask() = default;

    virtual Task* GetParent() noexcept override;
    virtual const Task* GetParent() const noexcept override;
protected:
    MeshTask* parent;
};
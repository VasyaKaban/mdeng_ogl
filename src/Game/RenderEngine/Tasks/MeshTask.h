#pragma once

#include "hrs/non_creatable.hpp"
#include "../TaskTree/Task.h"

class MaterialTask;

class MeshTask : hrs::non_movable, public Task
{
public:
    MeshTask(MaterialTask* _parent) noexcept;
    virtual ~MeshTask() = default;

    virtual Task* GetParent() noexcept override;
    virtual const Task* GetParent() const noexcept override;
protected:
    MaterialTask* parent;
};
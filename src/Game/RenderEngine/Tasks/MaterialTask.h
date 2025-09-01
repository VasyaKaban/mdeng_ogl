#pragma once

#include "hrs/non_creatable.hpp"
#include "../TaskTree/Task.h"

class GraphicsPipelineTask;

class MaterialTask : hrs::non_movable, public Task
{
public:
    MaterialTask(GraphicsPipelineTask* _parent) noexcept;
    virtual ~MaterialTask() = default;

    virtual Task* GetParent() noexcept override;
    virtual const Task* GetParent() const noexcept override;
protected:
    GraphicsPipelineTask* parent;
};
#pragma once

#include "../TaskTree/Task.h"
#include "Core/Render/Objects/Pipeline/Pipeline.h"

class RenderPassTask;

class GraphicsPipelineTask : public Task
{
public:
    GraphicsPipelineTask(RenderPassTask* _parent, Pipeline&& _handle) noexcept;
    virtual ~GraphicsPipelineTask() = default;

    virtual Task* GetParent() noexcept override;
    virtual const Task* GetParent() const noexcept override;

    Pipeline& GetHandle() noexcept;
    const Pipeline& GetHandle() const noexcept;
protected:
    RenderPassTask* parent;
    Pipeline handle;
};
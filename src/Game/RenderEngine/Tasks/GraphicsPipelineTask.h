#pragma once

#include "../TaskTree/Task.h"
#include "Core/Render/Objects//Pipeline.h"

class RenderPassTask;

class GraphicsPipelineTask : public TaskBase
{
public:
    GraphicsPipelineTask(Task<RenderPassTask>* _parent,
                         TaskKey&& key,
                         const Render::GraphicsPipelineInfo& info) noexcept;
    virtual ~GraphicsPipelineTask() = default;

    Render::Pipeline* GetHandle() const noexcept;
protected:
    std::unique_ptr<Render::Pipeline> handle;
};
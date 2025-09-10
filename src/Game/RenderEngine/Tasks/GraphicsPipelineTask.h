#pragma once

#include "../TaskTree/Task.h"
#include "Core/Render/Objects//Pipeline.h"

class RenderPassTask;

class GraphicsPipelineTask : public Task
{
public:
    GraphicsPipelineTask(RenderPassTask* _parent,
                         TaskKey&& key,
                         std::unique_ptr<Render::Pipeline>&& _handle) noexcept;
    virtual ~GraphicsPipelineTask() = default;

    Render::Pipeline* GetHandle() const noexcept;
protected:
    std::unique_ptr<Render::Pipeline> handle;
};
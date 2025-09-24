#include "GraphicsPipelineTask.h"
#include "RenderPassTask.h"
#include "../RenderEngine.h"
#include "Core/Render/Context.h"

GraphicsPipelineTask::GraphicsPipelineTask(RenderPassTask* _parent,
                                           TaskKey&& key,
                                           const Render::GraphicsPipelineInfo& info) noexcept
    : TaskBase(_parent, std::move(key)),
      handle(parent->GetRoot()->GetContext()->CreatePipelineUnique(info))
{}

Render::Pipeline* GraphicsPipelineTask::GetHandle() const noexcept
{
    return handle.get();
}
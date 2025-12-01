#include "GraphicsPipelineTask.h"
#include "RenderPassTask.h"
#include "../RenderEngine.h"
#include "Core/Render/Context.h"

GraphicsPipelineTask::GraphicsPipelineTask(RenderPassTask* _parent,
                                           TaskKey&& key,
                                           const Render::GraphicsPipelineInfo& info) noexcept
    : TaskBranch(_parent, std::move(key)),
      handle(static_cast<RenderEngine*>(GetParent()->GetRoot())
                 ->GetContext()
                 ->CreatePipelineUnique(info))
{}

Render::Pipeline* GraphicsPipelineTask::GetHandle() const noexcept
{
    return handle.get();
}
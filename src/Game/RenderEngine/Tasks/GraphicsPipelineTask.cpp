#include "GraphicsPipelineTask.h"
#include "RenderPassTask.h"

GraphicsPipelineTask::GraphicsPipelineTask(RenderPassTask* _parent,
                                           TaskKey&& key,
                                           std::unique_ptr<Render::Pipeline>&& _handle) noexcept
    : Task(_parent, std::move(key)),
      handle(std::move(_handle))
{}

Render::Pipeline* GraphicsPipelineTask::GetHandle() const noexcept
{
    return handle.get();
}
#include "RenderPassTask.h"
#include "../RenderQueue/RenderQueue.h"
#include "Core/Render/Context.h"
#include "../RenderEngine.h"

RenderPassTask::RenderPassTask(Task<RenderQueue>* _parent,
                               TaskKey&& key,
                               const Render::RenderPassInfo& info) noexcept
    : TaskBase(_parent, std::move(key)),
      handle(parent->GetRoot()->GetContext()->CreateRenderPass(info))
{}

Render::RenderPass* RenderPassTask::GetHandle() const noexcept
{
    return handle.get();
}
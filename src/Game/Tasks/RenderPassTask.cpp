#include "RenderPassTask.h"
#include "../RenderQueue/RenderQueue.h"
#include "Core/Render/Context.h"
#include "../RenderEngine.h"

RenderPassTask::RenderPassTask(RenderQueue* _parent,
                               TaskKey&& key,
                               const Render::RenderPassInfo& info) noexcept
    : TaskBranch(_parent, std::move(key)),
      handle(static_cast<RenderEngine*>(GetParent()->GetRoot())
                 ->GetContext()
                 ->CreateRenderPassUnique(info))
{}

Render::RenderPass* RenderPassTask::GetHandle() const noexcept
{
    return handle.get();
}
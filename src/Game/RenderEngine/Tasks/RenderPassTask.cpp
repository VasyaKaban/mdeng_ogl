#include "RenderPassTask.h"
#include "../RenderQueue/RenderQueue.h"

RenderPassTask::RenderPassTask(RenderQueue* _parent,
                               TaskKey&& key,
                               std::unique_ptr<Render::RenderPass>&& _handle) noexcept
    : Task(_parent, std::move(key)),
      handle(std::move(_handle))
{}

Render::RenderPass* RenderPassTask::GetHandle() const noexcept
{
    return handle.get();
}
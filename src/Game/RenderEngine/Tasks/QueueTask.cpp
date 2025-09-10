#include "QueueTask.h"
#include "../RenderEngine.h"

QueueTask::QueueTask(RenderEngine* _parent,
                     TaskKey&& key,
                     std::unique_ptr<Render::Queue>&& _handle) noexcept
    : Task(_parent, std::move(key)),
      handle(std::move(_handle))
{}

Render::Queue* QueueTask::GetHandle() const noexcept
{
    return handle.get();
}
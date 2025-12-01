#include "QueueTask.h"
#include "../RenderEngine.h"
#include "Core/Render/Context.h"
#include "../RenderEngine.h"

QueueTask::QueueTask(RenderEngine* _parent, TaskKey&& key, Render::Queue* queue) noexcept
    : TaskBranch(_parent, std::move(key)),
      handle(queue)
{}

Render::Queue* QueueTask::GetHandle() const noexcept
{
    return handle;
}
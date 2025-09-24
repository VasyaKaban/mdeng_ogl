#include "QueueTask.h"
#include "../RenderEngine.h"
#include "Core/Render/Context.h"
#include "../RenderEngine.h"

QueueTask::QueueTask(RenderEngine* _parent,
                     TaskKey&& key,
                     Render::QueueSpecialization spec) noexcept
    : TaskBase(_parent, std::move(key)),
      handle(parent->GetRoot()->GetContext()->GetQueue(spec))
{}

Render::Queue* QueueTask::GetHandle() const noexcept
{
    return handle;
}
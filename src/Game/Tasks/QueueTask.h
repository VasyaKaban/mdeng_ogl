#pragma once

#include "../TaskTree/Task.h"
#include "Core/Render/Objects/Queue.h"

class RenderEngine;

class QueueTask : public TaskBranch
{
public:
    QueueTask(RenderEngine* _parent, TaskKey&& key, Render::Queue* queue) noexcept;
    virtual ~QueueTask() = default;

    Render::Queue* GetHandle() const noexcept;
protected:
    Render::Queue* handle;
};
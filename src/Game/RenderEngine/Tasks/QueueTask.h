#pragma once

#include "../TaskTree/Task.h"
#include "Core/Render/Objects/Queue/Queue.h"

class RenderEngine;

class QueueTask : public Task
{
public:
    QueueTask(RenderEngine* _parent, Queue&& _handle) noexcept;
    virtual ~QueueTask() = default;

    virtual Task* GetParent() noexcept override;
    virtual const Task* GetParent() const noexcept override;

    Queue& GetHandle() noexcept;
    const Queue& GetHandle() const noexcept;
protected:
    RenderEngine* parent;
    Queue handle;
};
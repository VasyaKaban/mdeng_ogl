#pragma once

#include <memory>
#include "../TaskTree/Task.h"
#include "Core/Render/Objects/Queue.h"

class RenderEngine;

class QueueTask : public Task
{
public:
    QueueTask(RenderEngine* _parent,
              TaskKey&& key,
              std::unique_ptr<Render::Queue>&& _handle) noexcept;
    virtual ~QueueTask() = default;

    Render::Queue* GetHandle() const noexcept;
protected:
    std::unique_ptr<Render::Queue> handle;
};
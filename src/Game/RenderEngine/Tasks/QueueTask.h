#pragma once

#include <memory>
#include "../TaskTree/Task.h"
#include "Core/Render/Objects/Queue.h"

class RenderEngine;

class QueueTask : public TaskBase
{
public:
    QueueTask(Task<RenderEngine>* _parent,
              TaskKey&& key,
              Render::QueueSpecialization spec) noexcept;
    virtual ~QueueTask() = default;

    Render::Queue* GetHandle() const noexcept;
protected:
    std::unique_ptr<Render::Queue> handle;
};
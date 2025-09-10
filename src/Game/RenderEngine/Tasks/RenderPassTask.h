#pragma once

#include "../TaskTree/Task.h"
#include "Core/Render/Objects/RenderPass.h"

class RenderQueue;

class RenderPassTask : public Task
{
public:
    RenderPassTask(RenderQueue* _parent,
                   TaskKey&& key,
                   std::unique_ptr<Render::RenderPass>&& _handle) noexcept;
    virtual ~RenderPassTask() = default;

    Render::RenderPass* GetHandle() const noexcept;
protected:
    std::unique_ptr<Render::RenderPass> handle;
};
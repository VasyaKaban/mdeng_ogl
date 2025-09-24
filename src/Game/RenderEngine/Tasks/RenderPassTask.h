#pragma once

#include "../TaskTree/Task.h"
#include "Core/Render/Objects/RenderPass.h"

class RenderQueue;

class RenderPassTask : public TaskBase
{
public:
    RenderPassTask(RenderQueue* _parent,
                   TaskKey&& key,
                   const Render::RenderPassInfo& info) noexcept;
    virtual ~RenderPassTask() = default;

    Render::RenderPass* GetHandle() const noexcept;
protected:
    std::unique_ptr<Render::RenderPass> handle;
};
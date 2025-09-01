#pragma once

#include "../TaskTree/Task.h"
#include "Core/Render/Objects/RenderPass/RenderPass.h"

class RenderPass;
class RenderQueue;

class RenderPassTask : public Task
{
public:
    RenderPassTask(RenderQueue* _parent, RenderPass&& _handle) noexcept;
    virtual ~RenderPassTask() = default;

    virtual Task* GetParent() noexcept override;
    virtual const Task* GetParent() const noexcept override;

    RenderPass& GetHandle() noexcept;
    const RenderPass& GetHandle() const noexcept;
protected:
    RenderQueue* parent;
    RenderPass handle;
};
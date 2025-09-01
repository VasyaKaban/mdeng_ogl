#pragma once

#include "../Render.h"

namespace Render
{
    class RenderPass
    {
    public:
        virtual ~RenderPass()
        {}

        virtual void Begin(const CommandBuffer* cmd, const RenderPassBeginInfo& info) = 0;
        virtual void End(const CommandBuffer* cmd) = 0;

        virtual Context* GetContext() const noexcept = 0;
    };
};
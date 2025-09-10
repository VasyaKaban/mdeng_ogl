#pragma once

#include "../Render.h"
#include "Object.h"

namespace Render
{
    class RenderPass : public Object
    {
    public:
        virtual ~RenderPass()
        {}

        virtual void Begin(const CommandBuffer* cmd, const RenderPassBeginInfo& info) = 0;
        virtual void End(const CommandBuffer* cmd) = 0;
    };
};
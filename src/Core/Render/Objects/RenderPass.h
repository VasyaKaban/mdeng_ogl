#pragma once

#include "../Render.h"

namespace Render
{
    class CORE_API RenderPass
    {
    public:
        virtual ~RenderPass() = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};
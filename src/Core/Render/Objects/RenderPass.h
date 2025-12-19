#pragma once

#include "../Render.h"

namespace Render
{
    class RenderPass
    {
    public:
        virtual ~RenderPass()
        {}

        virtual Device* GetParent() const noexcept = 0;
    };
};
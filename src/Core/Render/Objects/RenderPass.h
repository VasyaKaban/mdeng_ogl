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
    };
};
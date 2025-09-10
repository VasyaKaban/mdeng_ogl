#pragma once

#include "../Render.h"
#include "Object.h"

namespace Render
{
    class CommandPool : public Object
    {
    public:
        virtual ~CommandPool()
        {}

        virtual CommandBuffer* Allocate() = 0;
    };
};
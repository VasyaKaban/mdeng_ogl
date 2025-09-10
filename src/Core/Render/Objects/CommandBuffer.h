#pragma once

#include "../Render.h"
#include "Object.h"

namespace Render
{
    class CommandBuffer : public Object
    {
    public:
        virtual ~CommandBuffer()
        {}

        virtual void Reset() = 0;
        virtual void Begin() = 0;
        virtual void End() = 0;
    };
};
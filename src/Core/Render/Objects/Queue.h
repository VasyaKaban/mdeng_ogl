#pragma once

#include "../Render.h"
#include "Object.h"

namespace Render
{
    class Queue : public Object
    {
    public:
        virtual ~Queue()
        {}

        virtual void Begin(QueueBeginInfo& info) = 0;
        virtual void Flush(QueueFlushInfo& info) = 0;
    };
};
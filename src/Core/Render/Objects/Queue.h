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

        virtual void Begin(const QueueBeginInfo& info) = 0;
        virtual void Flush(const QueueFlushInfo& info) = 0;

        virtual void WaitIdle() = 0;
    };
};
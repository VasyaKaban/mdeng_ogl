#pragma once

#include "../Render.h"

namespace Render
{
    class Queue
    {
    public:
        virtual ~Queue()
        {}

        virtual void Begin(const QueueBeginInfo& info) = 0;
        virtual void Flush(const QueueFlushInfo& info) = 0;

        virtual void WaitIdle() = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};
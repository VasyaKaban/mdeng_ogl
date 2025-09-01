#pragma once

#include "../Render.h"

namespace Render
{
    class Queue
    {
    public:
        virtual ~Queue()
        {}

        virtual void Begin(QueueBeginInfo& info) = 0;
        virtual void Flush(QueueFlushInfo& info) = 0;

        virtual Context* GetContext() const noexcept = 0;
    };
};
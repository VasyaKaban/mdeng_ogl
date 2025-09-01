#pragma once

#include "../Render.h"

namespace Render
{
    class CommandBuffer
    {
    public:
        virtual ~CommandBuffer()
        {}

        virtual void Reset() = 0;
        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual Context* GetContext() const noexcept = 0;
    };
};
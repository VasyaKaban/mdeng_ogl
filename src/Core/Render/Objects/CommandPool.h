#pragma once

#include "../Render.h"

namespace Render
{
    class CommandPool
    {
    public:
        virtual ~CommandPool()
        {}

        virtual CommandBuffer* Allocate() = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};
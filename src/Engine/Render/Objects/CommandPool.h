#pragma once

#include "../Render.h"

namespace Render
{
    class CORE_API CommandPool
    {
    public:
        virtual ~CommandPool() = 0;

        virtual CommandBuffer* Allocate() = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};
#pragma once

#include "../Render.h"

namespace Render
{
    class CORE_API Semaphore
    {
    public:
        virtual ~Semaphore() = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};
#pragma once

#include "../Render.h"

namespace Render
{
    class Semaphore
    {
    public:
        virtual ~Semaphore() {};

        virtual Device* GetParent() const noexcept = 0;
    };
};
#pragma once

#include "../Render.h"

namespace Render
{
    class Semaphore
    {
    public:
        virtual ~Semaphore()
        {}

        virtual Context* GetContext() const noexcept = 0;
    };
};
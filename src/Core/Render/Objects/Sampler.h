#pragma once

#include "../Render.h"

namespace Render
{
    class Sampler
    {
    public:
        virtual ~Sampler()
        {}

        virtual Context* GetContext() const noexcept = 0;
    };
};
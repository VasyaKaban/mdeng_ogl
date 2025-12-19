#pragma once

#include "../Render.h"

namespace Render
{
    class Sampler
    {
    public:
        virtual ~Sampler() {};

        virtual Device* GetParent() const noexcept = 0;
    };
};
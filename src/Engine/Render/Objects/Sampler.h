#pragma once

#include "../Render.h"

namespace Render
{
    class CORE_API Sampler
    {
    public:
        virtual ~Sampler() = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};
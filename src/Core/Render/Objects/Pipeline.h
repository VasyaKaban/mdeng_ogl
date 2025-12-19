#pragma once

#include "../Render.h"

namespace Render
{
    class Pipeline
    {
    public:
        virtual ~Pipeline() {};

        virtual Device* GetParent() const noexcept = 0;
    };
};
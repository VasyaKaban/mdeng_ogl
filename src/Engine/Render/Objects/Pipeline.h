#pragma once

#include "../Render.h"

namespace Render
{
    class CORE_API Pipeline
    {
    public:
        virtual ~Pipeline() = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};
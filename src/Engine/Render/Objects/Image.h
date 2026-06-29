#pragma once

#include "../Render.h"

namespace Render
{
    class CORE_API Image
    {
    public:
        virtual ~Image() = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};
#pragma once

#include "../Render.h"

namespace Render
{
    class CORE_API BufferView
    {
    public:
        virtual ~BufferView() = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};
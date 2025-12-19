#pragma once

#include "../Render.h"

namespace Render
{
    class BufferView
    {
    public:
        virtual ~BufferView() {};

        virtual Device* GetParent() const noexcept = 0;
    };
};
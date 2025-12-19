#pragma once

#include "../Render.h"

namespace Render
{
    class Image
    {
    public:
        virtual ~Image()
        {}

        virtual Device* GetParent() const noexcept = 0;
    };
};
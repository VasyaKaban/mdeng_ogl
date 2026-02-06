#pragma once

#include "../Render.h"

namespace Render
{
    class CORE_API ImageView
    {
    public:
        virtual ~ImageView() = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};
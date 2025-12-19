#pragma once

#include "../Render.h"

namespace Render
{
    class ImageView
    {
    public:
        virtual ~ImageView() {};

        virtual Device* GetParent() const noexcept = 0;
    };
};
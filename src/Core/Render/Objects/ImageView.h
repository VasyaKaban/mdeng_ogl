#pragma once

#include "../Render.h"

namespace Render
{
    class ImageView
    {
    public:
        virtual ~ImageView()
        {}

        virtual Context* GetContext() const noexcept = 0;
    };
};
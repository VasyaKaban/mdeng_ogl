#pragma once

#include "../Render.h"
#include "Object.h"

namespace Render
{
    class Image : public Object
    {
    public:
        virtual ~Image()
        {}

        virtual const ImageInfo& GetInfo() const noexcept = 0;
    };
};
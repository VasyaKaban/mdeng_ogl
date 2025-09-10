#pragma once

#include "../Render.h"
#include "Object.h"

namespace Render
{
    class ImageView : public Object
    {
    public:
        virtual ~ImageView() = 0;
    };

    inline ImageView::~ImageView()
    {}
};
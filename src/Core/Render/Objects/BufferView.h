#pragma once

#include "../Render.h"
#include "Object.h"

namespace Render
{
    class BufferView : public Object
    {
    public:
        virtual ~BufferView() = 0;
    };

    inline BufferView::~BufferView()
    {}
};
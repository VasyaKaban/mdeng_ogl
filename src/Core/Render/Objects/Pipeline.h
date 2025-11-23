#pragma once

#include "../Render.h"
#include "Object.h"

namespace Render
{
    class Pipeline : public Object
    {
    public:
        virtual ~Pipeline() {};
    };
};
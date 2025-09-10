#pragma once

#include "../Render.h"
#include "Object.h"

namespace Render
{
    class Semaphore : public Object
    {
    public:
        virtual ~Semaphore() = 0;
    };

    inline Semaphore::~Semaphore()
    {}
};
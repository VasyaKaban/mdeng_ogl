#pragma once

#include "../Render.h"
#include "Object.h"

namespace Render
{
    class Sampler : public Object
    {
    public:
        virtual ~Sampler() = 0;
    };

    inline Sampler::~Sampler()
    {}
};
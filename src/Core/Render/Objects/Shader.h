#pragma once

#include "../Render.h"
#include "Object.h"

namespace Render
{
    class Shader : public Object
    {
    public:
        virtual ~Shader() = 0;
    };

    inline Shader::~Shader()
    {}
};
#pragma once

#include "../Render.h"
#include "Object.h"

namespace Render
{
    class Fence : public Object
    {
    public:
        virtual ~Fence()
        {}

        virtual bool Wait(std::uint64_t timeout_ns) noexcept = 0;
        virtual FenceStatus GetStatus() const noexcept = 0;
    };
};
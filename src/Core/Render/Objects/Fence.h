#pragma once

#include "../Render.h"

namespace Render
{
    class Fence
    {
    public:
        virtual ~Fence()
        {}

        virtual bool Wait(std::uint64_t timeout_ns) noexcept = 0;
        virtual FenceStatus GetStatus() const noexcept = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};
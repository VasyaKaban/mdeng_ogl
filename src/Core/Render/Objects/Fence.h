#pragma once

#include "../Render.h"

namespace Render
{
    class CORE_API Fence
    {
    public:
        virtual ~Fence() = 0;

        virtual bool Wait(std::uint64_t timeout_ns) noexcept = 0;
        virtual bool Reset() noexcept = 0;
        virtual FenceStatus GetStatus() const noexcept = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};
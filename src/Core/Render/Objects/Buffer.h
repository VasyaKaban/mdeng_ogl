#pragma once

#include <span>
#include "../Render.h"
#include "Object.h"

namespace Render
{
    class Buffer : public Object
    {
    public:
        virtual ~Buffer()
        {}

        virtual std::byte* Map(const MappedRange& rng) = 0;
        virtual void Unmap() noexcept = 0;
        virtual void FlushMappedRange(std::span<const MappedRange> ranges) = 0;

        virtual std::uint64_t GetSize() const noexcept = 0;
    };
};
#pragma once

#include <span>
#include "../Render.h"

namespace Render
{
    class Buffer
    {
    public:
        virtual ~Buffer()
        {}

        virtual std::byte* Map(const MappedRange& rng,
                               BufferMapContentPolicy previous_content_policy) = 0;
        virtual void Unmap() noexcept = 0;
        virtual void FlushMappedRange(std::span<const MappedRange> ranges) = 0;
        virtual void InvalidateMappedRanges(std::span<const MappedRange> ranges) = 0;

        virtual std::uint64_t
        GetInnerMemoryOffset() const noexcept = 0; //OGL -> 0; VK -> offset within VkDeviceMemory

        virtual Device* GetParent() const noexcept = 0;
    };
};

/*
Mapping:
    Map with range
    OGL -> Map with range
    VK -> map the whole memory handle + also increment mapped memory counter when mapping new buffer on it and decrement on unmapping
*/

/*
Sequential buffers hint:
    Create buffer and allocate enough space for other buffers
    Buffer* CreateBufferSequentialHint(const BufferInfo& main_info, std::span<const BufferInfo> hint_buffer_infos, std::span<const MemoryReuest> requests);

    Buffer* CreateBufferWithSequentialHint(Buffer* main_buffer);
*/
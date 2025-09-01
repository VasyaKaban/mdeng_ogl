#pragma once

#include "../Render.h"

namespace Render
{
    class Buffer
    {
    public:
        virtual ~Buffer()
        {}

        virtual std::byte* Map(const MappedRange& rng) = 0;
        virtual void Unmap() noexcept = 0;
        virtual void FlushMappedRange(std::span<const MappedRange> ranges) = 0;

        virtual std::uint64_t GetSize() const noexcept = 0;

        virtual void CopyToBuffer(const CommandBuffer* cmd,
                                  const Buffer* dst,
                                  std::span<const BufferCopyRegion> regions) noexcept = 0;
        virtual void CopyToImage(const CommandBuffer* cmd,
                                 const Image* dst,
                                 std::span<const BufferImageCopyRegion> regions) = 0;

        virtual void Update(const CommandBuffer* cmd,
                            std::int64_t offset,
                            std::span<const std::byte*> data) noexcept = 0;

        virtual Context* GetContext() const noexcept = 0;
    };
};
#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Buffer.h"

namespace OpenGL
{
    class Buffer : public Render::Buffer, hrs::non_copyable, hrs::non_movable
    {
    public:
        Buffer(Context* _parent, const Render::BufferInfo& info);
        virtual ~Buffer() override;

        virtual std::byte* Map(const Render::MappedRange& rng) override;
        virtual void Unmap() noexcept override;
        virtual void FlushMappedRange(std::span<const Render::MappedRange> ranges) override;

        virtual std::uint64_t GetSize() const noexcept override;

        virtual void
        CopyToBuffer(const Render::CommandBuffer* cmd,
                     const Render::Buffer* dst,
                     std::span<const Render::BufferCopyRegion> regions) noexcept override;
        virtual void CopyToImage(const Render::CommandBuffer* cmd,
                                 const Render::Image* dst,
                                 std::span<const Render::BufferImageCopyRegion> regions) override;

        virtual void Update(const Render::CommandBuffer* cmd,
                            std::int64_t offset,
                            std::span<const std::byte*> data) noexcept override;

        GLHandle GetHandle() const noexcept;

        virtual Render::Context* GetContext() const noexcept override;
    private:
        Context* parent;
        GLHandle handle;
        GLsizeiptr size;
        GLbitfield flags;
    };
};
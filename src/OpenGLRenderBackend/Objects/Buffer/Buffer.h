#pragma once

#include "Core/Utils/NonCreatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Buffer.h"

namespace OpenGL
{
    class Buffer : public Render::Buffer, Core::NonCopyable, Core::NonMovable
    {
    public:
        Buffer(Device* _parent,
               const Render::BufferInfo& info,
               std::span<const std::uint32_t> desired_memory_type_indices);
        virtual ~Buffer() override;

        virtual std::byte* Map(const Render::MappedRange& rng) override;
        virtual void Unmap() noexcept override;
        virtual void FlushMappedRange(std::span<const Render::MappedRange> ranges) override;
        virtual void InvalidateMappedRanges(std::span<const Render::MappedRange> ranges) override;

        virtual std::uint64_t GetInnerMemoryOffset() const noexcept override;

        virtual Render::Device* GetParent() const noexcept override;

        GLHandle GetHandle() const noexcept;
    private:
        Device* parent;
        GLHandle handle;
        GLsizeiptr size;
        GLbitfield flags;
    };
};
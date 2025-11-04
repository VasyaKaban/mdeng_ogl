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

        GLHandle GetHandle() const noexcept;

        virtual Render::Context* GetContext() const noexcept override;
    private:
        Context* parent;
        GLHandle handle;
        GLsizeiptr size;
        GLbitfield flags;
    };
};
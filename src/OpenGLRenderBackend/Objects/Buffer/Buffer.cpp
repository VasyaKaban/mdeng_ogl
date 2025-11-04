#include "Buffer.h"
#include <stdexcept>
#include "../../Context/Context.h"

namespace OpenGL
{
    Buffer::Buffer(Context* _parent, const Render::BufferInfo& info)
        : parent(_parent)
    {
        GLHandle _handle = OGL_NULL_HANDLE;
        parent->GetLoader().CreateBuffers(1, &_handle);
        if(_handle == OGL_NULL_HANDLE)
            throw std::runtime_error("Failed to create buffer");

        auto full_flags = DecodeBufferFlagsToNative(info.flags);
        parent->GetLoader().NamedBufferStorage(_handle, info.size, nullptr, full_flags);

        handle = _handle;
        size = info.size;
        flags = full_flags;
    }

    Buffer::~Buffer()
    {
        parent->GetLoader().DeleteBuffers(1, &handle);
    }

    std::byte* Buffer::Map(const Render::MappedRange& rng)
    {
        if(handle == OGL_NULL_HANDLE)
            throw std::runtime_error("Usage of null buffer handle");

        //MAP_READ -> !GL_MAP_UNSYNCHRONIZED_BIT
        //FLUSH_EXPLICIT -> MAP_WRITE
        //

        //none -> device local
        //map read/write/persistence -> host visible(maybe host cached and maybe device local via BAR)
        //client storage -> host visible

        auto new_flags = flags & ~(GL_CLIENT_STORAGE_BIT);
        if(new_flags & GL_MAP_WRITE_BIT)
            new_flags |= GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_UNSYNCHRONIZED_BIT;

        std::byte* map_ptr = reinterpret_cast<std::byte*>(parent->GetLoader().MapNamedBufferRange(
            handle,
            rng.offset,
            rng.size,
            new_flags
            /*(flags | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_FLUSH_EXPLICIT_BIT) &
                ~(GL_CLIENT_STORAGE_BIT)*/));
        if(!map_ptr)
            throw std::runtime_error("Failed to map buffer");

        return map_ptr;
    }

    void Buffer::Unmap() noexcept
    {
        parent->GetLoader().UnmapNamedBuffer(handle);
    }

    void Buffer::FlushMappedRange(std::span<const Render::MappedRange> ranges)
    {
        if(!(flags & GL_MAP_WRITE_BIT))
            return;

        for(const auto& rng: ranges)
            parent->GetLoader().FlushMappedNamedBufferRange(handle, rng.offset, rng.size);
    }

    std::uint64_t Buffer::GetSize() const noexcept
    {
        return size;
    }

    GLHandle Buffer::GetHandle() const noexcept
    {
        return handle;
    }

    Render::Context* Buffer::GetContext() const noexcept
    {
        return parent;
    }
};
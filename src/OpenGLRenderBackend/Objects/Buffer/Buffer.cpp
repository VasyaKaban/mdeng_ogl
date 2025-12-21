#include "Buffer.h"
#include <stdexcept>
#include "../Device/Device.h"
#include "../PhysicalDevice/PhysicalDevice.h"

namespace OpenGL
{
    Buffer::Buffer(Device* _parent,
                   const Render::BufferInfo& info,
                   std::span<const std::uint32_t> desired_memory_type_indices)
        : parent(_parent)
    {
        GLHandle _handle = OGL_NULL_HANDLE;
        parent->GetLoader().CreateBuffers(1, &_handle);
        if(_handle == OGL_NULL_HANDLE)
            throw std::runtime_error("Failed to create buffer");

        GLbitfield full_flags = 0;
        if(!desired_memory_type_indices.empty())
        {
            const Render::MemoryType& mem_type =
                parent->GetParent()->GetProperties().memory_types[desired_memory_type_indices[0]];

            full_flags = DecodeBufferStorageFlags(mem_type.memory_type_flags, info.map_usage);
        }

        parent->GetLoader().NamedBufferStorage(_handle, info.size, nullptr, full_flags);

        handle = _handle;
        size = info.size;
        flags = full_flags;
    }

    Buffer::~Buffer()
    {
        parent->GetLoader().DeleteBuffers(1, &handle);
    }

    std::byte* Buffer::Map(const Render::MappedRange& rng,
                           Render::BufferMapContentPolicy previous_content_policy)
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
            new_flags |= GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_UNSYNCHRONIZED_BIT |
                         (previous_content_policy == Render::BufferMapContentPolicy::Discard ?
                              GL_MAP_INVALIDATE_RANGE_BIT :
                              0);

        if(new_flags & GL_MAP_READ_BIT)
            new_flags &= ~(GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

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

    void Buffer::InvalidateMappedRanges(std::span<const Render::MappedRange> ranges)
    {
        //noop??? -> resolve via barrier
    }

    std::uint64_t Buffer::GetInnerMemoryOffset() const noexcept
    {
        return 0;
    }

    Render::Device* Buffer::GetParent() const noexcept
    {
        return parent;
    }

    GLHandle Buffer::GetHandle() const noexcept
    {
        return handle;
    }
};
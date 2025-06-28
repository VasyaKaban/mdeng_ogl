#include "Buffer.h"
#include "../../Context/Context.h"
#include <stdexcept>

Buffer::Buffer() noexcept
    : parent(nullptr),
      handle(GL_NULL_HANDLE),
      map_ptr(nullptr),
      size(0),
      flags(0)
{}

Buffer::Buffer(Context* _parent, const BufferInfo& info, const BufferInitInfo& init_info)
    : parent(_parent),
      map_ptr(nullptr)
{
    GLHandle _handle = GL_NULL_HANDLE;
    parent->GetLoader().CreateBuffers(1, &_handle);
    if(_handle == GL_NULL_HANDLE)
        throw std::runtime_error("Failed to create buffer");

    auto full_flags = info.flags | GL_CLIENT_STORAGE_BIT;
    parent->GetLoader().NamedBufferStorage(_handle, info.size, init_info.init_data, full_flags);

    handle = _handle;
    size = info.size;
    flags = full_flags;
}

Buffer::~Buffer()
{
    destroy();
}

Buffer::Buffer(Buffer&& buffer) noexcept
    : parent(buffer.parent),
      handle(std::exchange(buffer.handle, GL_NULL_HANDLE)),
      map_ptr(std::exchange(buffer.map_ptr, nullptr)),
      size(buffer.size),
      flags(buffer.flags)
{}

Buffer& Buffer::operator=(Buffer&& buffer) noexcept
{
    destroy();

    parent = buffer.parent;
    handle = std::exchange(buffer.handle, GL_NULL_HANDLE);
    map_ptr = std::exchange(buffer.map_ptr, nullptr);
    size = buffer.size;
    flags = buffer.flags;

    return *this;
}

std::byte* Buffer::Map()
{
    if(map_ptr)
        return map_ptr;

    if(handle == GL_NULL_HANDLE)
        throw std::runtime_error("Usage of null buffer handle");

    map_ptr = reinterpret_cast<std::byte*>(parent->GetLoader().MapNamedBufferRange(
        handle,
        0,
        size,
        (flags | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_FLUSH_EXPLICIT_BIT) &
            ~(GL_CLIENT_STORAGE_BIT)));
    if(!map_ptr)
        throw std::runtime_error("Failed to map buffer");

    return map_ptr;
}

void Buffer::FlushMappedRange(std::span<const MappedRange> ranges)
{
    if(!map_ptr)
        throw std::runtime_error("Usage of unmapped buffer");

    for(const auto& rng: ranges)
        parent->GetLoader().FlushMappedNamedBufferRange(handle, rng.offset, rng.size);
}

std::byte* Buffer::GetMappedPtr() noexcept
{
    return map_ptr;
}

const std::byte* Buffer::GetMappedPtr() const noexcept
{
    return map_ptr;
}

bool Buffer::IsCreated() const noexcept
{
    return handle != GL_NULL_HANDLE;
}

void Buffer::BindUniformBuffer(const BufferBindDesc& bind_desc) const noexcept
{
    parent->GetLoader().BindBufferRange(GL_UNIFORM_BUFFER,
                                        bind_desc.index,
                                        handle,
                                        bind_desc.offset,
                                        size);
}

void Buffer::BindShaderStorageBuffer(const BufferBindDesc& bind_desc) const noexcept
{
    parent->GetLoader().BindBufferRange(GL_SHADER_STORAGE_BUFFER,
                                        bind_desc.index,
                                        handle,
                                        bind_desc.offset,
                                        size);
}

void Buffer::destroy() noexcept
{
    if(IsCreated())
    {
        if(map_ptr)
            parent->GetLoader().UnmapNamedBuffer(handle);

        parent->GetLoader().DeleteBuffers(1, &handle);
    }
}
#pragma once

#include <cstddef>
#include <type_traits>
#include <span>
#include "../../../hrs/non_creatable.hpp"
#include "../../Common.h"

class Context;

enum BufferFlagBits
{
    MapRead = GL_MAP_READ_BIT,
    MapWrite = GL_MAP_WRITE_BIT,
    PersistentMapping = GL_MAP_PERSISTENT_BIT,
    CoherentMapping = GL_MAP_COHERENT_BIT,
    //GL_CLIENT_STORAGE_BIT
};

using BufferFlags = std::underlying_type_t<BufferFlagBits>;

struct BufferInfo
{
    GLsizeiptr size;
    BufferFlags flags;
};

struct BufferInitInfo
{
    const std::byte* init_data;
};

struct BufferBindDesc
{
    GLuint index;
    GLintptr offset;
    GLsizeiptr size;
};

struct MappedRange
{
    GLintptr offset;
    GLsizei size;
};

class Buffer : hrs::non_copyable
{
    friend class Pipeline;
public:
    Buffer() noexcept;
    Buffer(Context* _parent, const BufferInfo& info, const BufferInitInfo& init_info);
    ~Buffer();
    Buffer(Buffer&& buffer) noexcept;
    Buffer& operator=(Buffer&& buffer) noexcept;

    std::byte* Map();
    void FlushMappedRange(std::span<const MappedRange> ranges);

    std::byte* GetMappedPtr() noexcept;
    const std::byte* GetMappedPtr() const noexcept;

    bool IsCreated() const noexcept;

    void BindUniformBuffer(const BufferBindDesc& bind_desc) const noexcept;
    void BindShaderStorageBuffer(const BufferBindDesc& bind_desc) const noexcept;
private:
    void destroy() noexcept;
private:
    Context* parent;
    GLHandle handle;
    std::byte* map_ptr;
    GLsizeiptr size;
    GLbitfield flags;
};
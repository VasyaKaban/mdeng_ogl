#pragma once

#include "hrs/non_creatable.hpp"
#include "Core/Render/Objects/Queue.h"
#include "Core/Render/Objects/CommandPool.h"
#include "Core/Render/Objects/CommandBuffer.h"
#include "TransferOperation.h"

class RenderEngine;

struct TransferStorageInfo
{
    std::uint64_t buffer_size;
};

class TransferStorage : hrs::non_copyable, hrs::non_movable
{
public:
    TransferStorage(RenderEngine* _parent, const TransferStorageInfo& info);
    ~TransferStorage();

    void Transfer(const TransferBufferOperation& op);
    void Transfer(const TransferImageOperation& op);
    void Transfer(const TransferCallbackOperation& op);

    std::uint64_t MaxTransferRegionSize() const noexcept;
    bool CanTransfer(std::uint64_t size) const noexcept;

    void Flush();

    Render::Queue* GetQueue() const noexcept;
private:
    void start_write();
private:
    RenderEngine* parent;
    Render::Queue* queue;
    std::unique_ptr<Render::CommandPool> command_pool;
    std::unique_ptr<Render::CommandBuffer> command_buffer;
    std::unique_ptr<Render::Buffer> buffer;
    std::byte* mapped_ptr;
    std::uint64_t buffer_offset;
    bool write_started;

    std::vector<Render::BufferCopyRegion> buffer_regions_cache;
    std::vector<Render::BufferImageCopyRegion> image_regions_cache;
};
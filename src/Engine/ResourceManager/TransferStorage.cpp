#include "TransferStorage.h"
#include "../RenderEngine/RenderEngine.h"
#include "Core/Render/Context.h"
#include <stdexcept>
#include <format>

TransferStorage::TransferStorage(RenderEngine* _parent, const TransferStorageInfo& info)
    : parent(_parent),
      queue(_parent->GetContext()->GetQueue(Render::QueueSpecializationFlagBits::TransferSpec)),
      command_pool(
          parent->GetContext()->CreateCommandPoolUnique(Render::CommandPoolInfo{.queue = queue})),
      command_buffer(command_pool->AllocateUnique()),
      mapped_ptr(nullptr),
      buffer_offset(0),
      write_started(false)
{
    Render::BufferInfo buffer_info = {.size = info.buffer_size,
                                      .mapping_flags =
                                          Render::BufferMappingFlagBits::MapWrite |
                                          Render::BufferMappingFlagBits::PersistentMapping};

    buffer = parent->GetContext()->CreateBufferUnique(buffer_info);
    mapped_ptr = buffer->Map(Render::MappedRange{.offset = 0, .size = info.buffer_size});

    buffer_regions_cache.reserve(16);
    image_regions_cache.reserve(16);
}

TransferStorage::~TransferStorage()
{
    Flush();
}

void TransferStorage::Transfer(const TransferBufferOperation& op)
{
    if(op.regions.empty())
        return;

    start_write();

    for(const auto& reg: op.regions)
    {
        if(!CanTransfer(reg.size))
            throw std::runtime_error(std::format(
                "Failed to transfer region with size bigger than storage is. Storage size = {}",
                buffer->GetSize()));

        if((buffer->GetSize() - buffer_offset) >= reg.size)
        {
            std::memcpy(mapped_ptr + buffer_offset, reg.data, reg.size);
            buffer_regions_cache.push_back(Render::BufferCopyRegion{.src_offset = buffer_offset,
                                                                    .dst_offset = reg.offset,
                                                                    .size = reg.size});
            buffer_offset += reg.size;
        }
        else
        {
            if(!buffer_regions_cache.empty())
            {
                command_buffer->CopyBufferToBuffer(buffer.get(), op.buffer, buffer_regions_cache);
                buffer_regions_cache.clear();
            }

            Flush();
            start_write();
        }
    }

    if(!buffer_regions_cache.empty())
    {
        command_buffer->CopyBufferToBuffer(buffer.get(), op.buffer, buffer_regions_cache);
        buffer_regions_cache.clear();
    }
}

void TransferStorage::Transfer(const TransferImageOperation& op)
{
    if(op.regions.empty())
        return;

    start_write();

    Render::Format format = op.image->GetInfo().format;

    for(const auto& reg: op.regions)
    {
        auto region_size = Render::GetFormatRegionSize(format, reg);
        if(!CanTransfer(region_size))
            throw std::runtime_error(std::format(
                "Failed to transfer region with size bigger than storage is. Storage size = {}",
                buffer->GetSize()));

        if((buffer->GetSize() - buffer_offset) > region_size)
        {
            std::memcpy(mapped_ptr + buffer_offset, reg.data, region_size);
            image_regions_cache.push_back(
                Render::BufferImageCopyRegion{.buffer_offset = buffer_offset,
                                              .buffer_row_length = reg.buffer_row_length,
                                              .buffer_image_height = reg.buffer_image_height,
                                              .subresource_layers = reg.subresource_layers,
                                              .offset = reg.offset,
                                              .extent = reg.extent});
            buffer_offset += region_size;
        }
        else
        {
            if(!image_regions_cache.empty())
            {
                command_buffer->CopyBufferToImage(buffer.get(), op.image, image_regions_cache);
                image_regions_cache.clear();
            }

            Flush();
            start_write();
        }
    }

    if(!image_regions_cache.empty())
    {
        command_buffer->CopyBufferToImage(buffer.get(), op.image, image_regions_cache);
        image_regions_cache.clear();
    }
}

void TransferStorage::Transfer(const TransferCallbackOperation& op)
{
    start_write();

    op.cback(command_buffer.get());
}

std::uint64_t TransferStorage::MaxTransferRegionSize() const noexcept
{
    return buffer->GetSize();
}

bool TransferStorage::CanTransfer(std::uint64_t size) const noexcept
{
    return buffer->GetSize() >= size;
}

void TransferStorage::Flush()
{
    if(!write_started)
        return;

    command_buffer->End();

    Render::CommandBuffer* cmd = command_buffer.get();
    const Render::QueueFlushInfo info = {.signal_fence = nullptr,
                                         .signal_seamphores = {},
                                         .command_buffers = {&cmd, 1}};

    Render::MappedRange range = {.offset = 0, .size = buffer_offset};
    buffer->FlushMappedRange({&range, 1});
    queue->Flush(info);
    queue->WaitIdle();
    write_started = false;
}

Render::Queue* TransferStorage::GetQueue() const noexcept
{
    return queue;
}

void TransferStorage::start_write()
{
    if(!write_started)
    {
        const Render::QueueBeginInfo info = {.wait_seamphores = {}};
        queue->Begin(info);
        command_buffer->Begin();
    }
}
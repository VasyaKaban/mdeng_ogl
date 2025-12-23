#include "TransferStorage.h"
#include "../RenderEngine/RenderEngine.h"
#include "Core/Render/Context.h"
#include "Core/Render/Objects/Buffer.h"
#include <stdexcept>
#include <format>

namespace Engine
{
    constexpr static int get_memory_type_score(const Render::MemoryType& type) noexcept
    {
        constexpr std::pair<Render::MemoryTypePropertyFlags, int> attributes[] = {
            std::pair{Render::MemoryTypePropertyFlagBits::DeviceLocal, -1},
            std::pair{Render::MemoryTypePropertyFlagBits::HostMappingReadable, -1},
            std::pair{Render::MemoryTypePropertyFlagBits::HostCoherent, -1},
            std::pair{Render::MemoryTypePropertyFlagBits::HostCached, 1}};

        int score = 0;
        for(const auto& [property, score_diff]: attributes)
        {
            if(type.memory_type_flags & property)
                score += score_diff;
        }

        return score;
    }

    TransferStorage::TransferStorage(RenderEngine* _parent, const TransferStorageInfo& info)
        : parent(_parent),
          queue(parent->GetContext()->GetQueue(info.queue_info)),
          queue_family_index(info.queue_info.family_index),
          command_pool(parent->GetContext()->CreateCommandPoolUnique(
              Render::CommandPoolInfo{.flags = Render::CommandPoolFlagBits::TransientCommandPoolBit,
                                      .queue_family_index = info.queue_info.family_index})),
          command_buffer(command_pool->AllocateUnique()),
          buffer_size(info.buffer_size),
          mapped_ptr(nullptr),
          buffer_offset(0),
          write_started(false),
          should_remap(!parent->GetContext()->GetProperties().persistent_mapping_used)
    {
        const Render::BufferInfo buffer_info = {
            .size = info.buffer_size,
            .usage = Render::BufferUsageFlagBits::BufferUsageTransferSource};

        const auto& available_memory_types = parent->GetContext()->GetProperties().memory_types;
        std::vector<std::pair<std::uint32_t, int>> memory_type_indices_pairs;
        for(std::uint32_t i = 0; i < available_memory_types.size(); i++)
        {
            if(available_memory_types[i].memory_type_flags &
               (Render::MemoryTypePropertyFlagBits::HostMappingWritable))
            {
                memory_type_indices_pairs.push_back(
                    {i, get_memory_type_score(available_memory_types[i])});
            }
        }

        std::ranges::sort(memory_type_indices_pairs,
                          std::ranges::greater{},
                          &std::pair<std::uint32_t, int>::second);

        std::vector<std::uint32_t> memory_type_indices;
        memory_type_indices.reserve(memory_type_indices_pairs.size());
        for(const auto& [index, _]: memory_type_indices_pairs)
            memory_type_indices.push_back(index);

        buffer = parent->GetContext()->CreateBufferUnique(buffer_info, memory_type_indices);

        if(!should_remap)
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
                    buffer_size));

            if((buffer_size - buffer_offset) >= reg.size)
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
                    command_buffer->CopyBufferToBuffer(buffer.get(),
                                                       op.buffer,
                                                       buffer_regions_cache);
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

        for(const auto& reg: op.regions)
        {
            auto region_size = Render::GetFormatRegionSize(op.format, reg);
            if(!CanTransfer(region_size))
                throw std::runtime_error(std::format(
                    "Failed to transfer region with size bigger than storage is. Storage size = {}",
                    buffer_size));

            if((buffer_size - buffer_offset) >= region_size)
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

    Render::CommandBuffer* TransferStorage::GetCommandBuffer() noexcept
    {
        start_write();

        return command_buffer.get();
    }

    std::uint64_t TransferStorage::MaxTransferRegionSize() const noexcept
    {
        return buffer_size;
    }

    bool TransferStorage::CanTransfer(std::uint64_t size) const noexcept
    {
        return buffer_size >= size;
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
        if(should_remap)
            buffer->Unmap();

        queue->Flush(info);
        queue->WaitIdle();
        write_started = false;
    }

    Render::Queue* TransferStorage::GetQueue() const noexcept
    {
        return queue;
    }

    std::uint32_t TransferStorage::GetQueueFamilyIndex() const noexcept
    {
        return queue_family_index;
    }

    Render::AccessFlags TransferStorage::GetTransferAccessFlags() const noexcept
    {
        return Render::AccessFlagBits::AccessTransferWiteBit;
    }

    Render::PipelineStageFlags TransferStorage::GetTransferPipelineStages() const noexcept
    {
        return Render::PipelineStageFlagBits::TransferPipelineStageBit;
    }

    Render::ImageLayout TransferStorage::GetTransferImageLayout() const noexcept
    {
        return Render::ImageLayout::TransferSourceOptimal;
    }

    void TransferStorage::start_write()
    {
        if(!write_started)
        {
            if(should_remap)
                mapped_ptr = buffer->Map(Render::MappedRange{.offset = 0, .size = buffer_size});

            const Render::QueueBeginInfo info = {.wait_seamphores = {}};
            queue->Begin(info);
            command_buffer->Begin(Render::CommandBufferBeginInfo{
                .flags = Render::CommandBufferUsageFlagBits::CommandBufferUsageOneTimeSubmitBit});
        }
    }
};
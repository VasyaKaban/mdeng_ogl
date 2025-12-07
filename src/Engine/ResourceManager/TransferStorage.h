#pragma once

#include "hrs/non_creatable.hpp"
#include "Core/Render/Objects/Queue.h"
#include "Core/Render/Objects/CommandPool.h"
#include "Core/Render/Objects/CommandBuffer.h"
#include "TransferOperation.h"

namespace Engine
{
    class RenderEngine;

    struct TransferStorageInfo
    {
        std::uint64_t buffer_size;
        Render::QueueInfo queue_info;
    };

    class TransferStorage : hrs::non_copyable, hrs::non_movable
    {
    public:
        TransferStorage(RenderEngine* _parent, const TransferStorageInfo& info);
        ~TransferStorage();

        void Transfer(const TransferBufferOperation& op);
        void Transfer(const TransferImageOperation& op);
        Render::CommandBuffer* GetCommandBuffer() noexcept;

        std::uint64_t MaxTransferRegionSize() const noexcept;
        bool CanTransfer(std::uint64_t size) const noexcept;

        void Flush();

        Render::Queue* GetQueue() const noexcept;

        //for pipeline barriers
        std::uint32_t GetQueueFamilyIndex() const noexcept;
        Render::AccessFlags GetTransferAccessFlags() const noexcept;
        Render::PipelineStageFlags GetTransferPipelineStages() const noexcept;
        Render::ImageLayout GetTransferImageLayout() const noexcept;
    private:
        void start_write();
    private:
        RenderEngine* parent;
        Render::Queue* queue;
        std::uint32_t queue_family_index;
        std::unique_ptr<Render::CommandPool> command_pool;
        std::unique_ptr<Render::CommandBuffer> command_buffer;
        std::unique_ptr<Render::Buffer> buffer;
        std::uint64_t buffer_size;
        std::byte* mapped_ptr;
        std::uint64_t buffer_offset;
        bool write_started;
        bool should_remap;

        std::vector<Render::BufferCopyRegion> buffer_regions_cache;
        std::vector<Render::BufferImageCopyRegion> image_regions_cache;
    };
};
#include "TransferChannel.h"
#include "TransferQueue.h"
#include "../RenderEngine.h"
#include "Core/Render/Objects/Image/Image.h"
#include <stdexcept>

TransferChannel::TransferChannel(TransferQueue* _parent, const TransferChannelInfo& info)
    : parent(_parent),
      wait_on_fence(nullptr),
      offset(0),
      regions_pool(info.regions_block_size, info.regions_reserve)
{
    const BufferInfo buffer_info = {.size = info.size,
                                    .flags = BufferFlagBits::PersistentMapping |
                                             BufferFlagBits::MapWrite};
    buffer = Buffer(RenderEngine::GetInstance()->GetContext(), buffer_info);
    mapped_ptr =
        buffer.Map(MappedRange{.offset = 0, .size = static_cast<GLsizei>(buffer_info.size)});
}

TransferChannel::~TransferChannel()
{
    if(mapped_ptr)
        buffer.Unmap();
}

void TransferChannel::Evaluate(EvaluateDesc& eval_desc)
{
    if(!regions_pool.pick())
        return;

    if(wait_on_fence)
    {
        if(!wait_on_fence->Wait(std::numeric_limits<std::uint64_t>::max()))
            throw std::runtime_error("Failed to wait on transfer channel fence");
    }

    offset = 0;

    TransferRegion* reg;
    bool buffer_is_full = false;
    bool partially_sent_region = false;
    while(!buffer_is_full && (reg = regions_pool.pick()))
    {
        if(std::holds_alternative<TransferBufferRegion>(reg->data))
        {
            if(offset == buffer.GetSize())
            {
                buffer_is_full = true;
                partially_sent_region = true;
            }
            else
            {
                TransferBufferRegion& buffer_reg = std::get<TransferBufferRegion>(reg->data);

                auto avail_space = buffer.GetSize() - offset;
                GLsizeiptr to_send_space = avail_space;

                if(avail_space >= buffer_reg.data.size())
                    to_send_space = buffer_reg.data.size();
                else if(avail_space < buffer_reg.data.size())
                {
                    buffer_is_full = true;
                    partially_sent_region = true;
                }

                std::memcpy(mapped_ptr + offset, buffer_reg.data.data(), to_send_space);

                const BufferCopyRegion copy_reg = {.src_offset = offset,
                                                   .dst_offset = buffer_reg.offset,
                                                   .size = to_send_space};

                buffer.CopyToBuffer(eval_desc.cmd, buffer_reg.buffer, {&copy_reg, 1});

                offset += to_send_space;
            }
        }
        else if(std::holds_alternative<TransferImageRegion>(reg->data))
        {
            TransferImageRegion& image_reg = std::get<TransferImageRegion>(reg->data);

            if(!image_reg.prefer_image_host_copy && offset == buffer.GetSize())
            {
                buffer_is_full = true;
                partially_sent_region = true;
            }
            else
            {
                //auto region_size =
                //    GetFormatRegionSize(image_reg.image->GetInfo().format, image_reg.extent);

                auto region_size = image_reg.data.size();
                if(!CanTransfer(region_size))
                    throw std::runtime_error(
                        "Failed to transfer Image region. It exceeds the limit.");

                const auto& image_info = image_reg.image->GetInfo();
                std::uint16_t format_texel_alignment = GetFormatTexelAlignment(image_info.format);
                GLintptr aligned_offset;
                {
                    GLintptr den = offset % format_texel_alignment;
                    if(den != 0)
                        offset += format_texel_alignment - den;
                }

                if(!image_reg.prefer_image_host_copy && offset > buffer.GetSize())
                {
                    buffer_is_full = true;
                    partially_sent_region = true;
                }
                else
                {
                    if(image_reg.prefer_image_host_copy)
                    {
                        const MemoryImageCopyRegion copy_reg = {
                            .data = image_reg.data.data(),
                            .buffer_row_length = image_reg.buffer_row_length,
                            .buffer_image_height = image_reg.buffer_image_height,
                            .subresource_layers = image_reg.subresource_layers,
                            .offset = image_reg.offset,
                            .extent = image_reg.extent,
                            .data_format = image_reg.data_format,
                            .data_type = image_reg.data_type};

                        image_reg.image->Update(eval_desc.cmd, {&copy_reg, 1});
                    }
                    else
                    {
                        auto avail_space = buffer.GetSize() - offset;

                        if(avail_space < region_size)
                        {
                            buffer_is_full = true;
                            partially_sent_region = true;
                        }
                        else
                        {
                            std::memcpy(mapped_ptr + offset, image_reg.data.data(), region_size);

                            const BufferImageCopyRegion copy_reg = {
                                .buffer_offset = offset,
                                .buffer_row_length = image_reg.buffer_row_length,
                                .buffer_image_height = image_reg.buffer_image_height,
                                .subresource_layers = image_reg.subresource_layers,
                                .offset = image_reg.offset,
                                .extent = image_reg.extent,
                                .data_format = image_reg.data_format,
                                .data_type = image_reg.data_type};

                            buffer.CopyToImage(eval_desc.cmd, image_reg.image, {&copy_reg, 1});

                            offset += region_size;
                        }
                    }
                }
            }
        }
        else if(std::holds_alternative<TransferCallback>(reg->data))
        {
            TransferCallback& cback = std::get<TransferCallback>(reg->data);
            cback();
        }

        if(!partially_sent_region)
            regions_pool.pop();
    }

    wait_on_fence = parent->GetCurrentFence();
}

Task* TransferChannel::GetParent() noexcept
{
    return parent;
}

const Task* TransferChannel::GetParent() const noexcept
{
    return parent;
}

void TransferChannel::Reserve(std::size_t size)
{
    regions_pool.reserve_next(size);
}

void TransferChannel::Transfer(std::span<const TransferRegion> regions)
{
    regions_pool.reserve_next(regions.size());
    for(auto& region: regions)
        regions_pool.acquire() = region;
}

bool TransferChannel::CanTransfer(GLintptr size) const noexcept
{
    return buffer.GetSize() >= size;
}

bool TransferChannel::CanTransfer(Format format, const BufferImageCopyRegion& reg) const noexcept
{
    return buffer.GetSize() >= GetFormatRegionSize(format, reg);
}
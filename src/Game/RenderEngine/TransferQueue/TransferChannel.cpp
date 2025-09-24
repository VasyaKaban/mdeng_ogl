#include "TransferChannel.h"
#include "TransferQueue.h"
#include "Core/Render/Objects/Buffer.h"
#include "Core/Render/Objects/Image.h"

TransferChannel::TransferChannel(TransferQueue* _parent,
                                 TaskKey&& key,
                                 const TransferChannelInfo& info)
    : TaskBase(_parent, std::move(key)),
      staging_regions(info.pool_block_size, info.pool_blocks_reserve)
{}

TransferChannel::~TransferChannel()
{}

EvaluateDesc TransferChannel::Begin(const EvaluateDesc& eval_desc)
{
    if(!staging_regions.pick())
        return eval_desc;

    TransferRegion* reg;
    while((reg = staging_regions.pick()))
    {
        if(std::holds_alternative<TransferBufferOperation>(reg->op))
        {
            TransferBufferOperation& op = std::get<TransferBufferOperation>(reg->op);

            op.buffer->Update(eval_desc.cmd, op.regions);
        }
        else if(std::holds_alternative<TransferImageOperation>(reg->op))
        {
            TransferImageOperation& op = std::get<TransferImageOperation>(reg->op);

            op.image->Update(eval_desc.cmd, op.regions);
        }
        else if(std::holds_alternative<TransferCallbackOperation>(reg->op))
        {
            TransferCallbackOperation& op = std::get<TransferCallbackOperation>(reg->op);
            op.cback();
        }

        staging_regions.pop();
    }

    return eval_desc;
}

void TransferChannel::End([[maybe_unused]] const EvaluateDesc& eval_desc)
{
    //noop
}

void TransferChannel::Transfer(TransferBufferOperation&& op)
{
    if(!op.regions.empty())
    {
        TransferRegion& reg = staging_regions.acquire();
        reg.op = std::move(op);
    }
}

void TransferChannel::Transfer(TransferImageOperation&& op)
{
    if(!op.regions.empty())
    {
        TransferRegion& reg = staging_regions.acquire();
        reg.op = std::move(op);
    }
}

void TransferChannel::Transfer(TransferCallbackOperation&& op)
{
    TransferRegion& reg = staging_regions.acquire();
    reg.op = std::move(op);
}

void TransferChannel::Reserve(std::size_t size)
{
    staging_regions.reserve_next(size);
}

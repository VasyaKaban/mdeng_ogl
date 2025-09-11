#include "TransferChannel.h"
#include "TransferQueue.h"
#include "Core/Render/Objects/Buffer.h"
#include "Core/Render/Objects/Image.h"

TransferChannel::TransferChannel(Task<TransferQueue>* _parent,
                                 TaskKey&& key,
                                 const TransferChannelStateInfo& info)
    : TaskBase(_parent, std::move(key)),
      state(new TransferChannelState(info))
{}

TransferChannel::~TransferChannel()
{}

EvaluateDesc TransferChannel::Begin(const EvaluateDesc& eval_desc)
{
    TransferChannelState& state_ref = *state;

    if(!state_ref.staging_regions.pick())
        return eval_desc;

    TransferRegion* reg;
    while((reg = state_ref.staging_regions.pick()))
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
    }

    return eval_desc;
}

void TransferChannel::End([[maybe_unused]] const EvaluateDesc& eval_desc)
{
    //noop
}

hrs::rc_ptr<TransferChannelState> TransferChannel::GetState() const noexcept
{
    return state.Get();
}
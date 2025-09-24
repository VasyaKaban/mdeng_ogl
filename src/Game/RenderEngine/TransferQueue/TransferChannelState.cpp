#include "TransferChannelState.h"

/*TransferChannelState::TransferChannelState(const TransferChannelStateInfo& info)
    : staging_regions(info.pool_block_size, info.pool_blocks_reserve),
      detached(false)
{}

TransferChannelState::~TransferChannelState()
{}

void TransferChannelState::Transfer(TransferBufferOperation&& op)
{
    if(!op.regions.empty() && !detached)
    {
        TransferRegion& reg = staging_regions.acquire();
        reg.op = std::move(op);
    }
}

void TransferChannelState::Transfer(TransferImageOperation&& op)
{
    if(!op.regions.empty() && !detached)
    {
        TransferRegion& reg = staging_regions.acquire();
        reg.op = std::move(op);
    }
}

void TransferChannelState::Transfer(TransferCallbackOperation&& op)
{
    if(!detached)
    {
        TransferRegion& reg = staging_regions.acquire();
        reg.op = std::move(op);
    }
}

void TransferChannelState::Reserve(std::size_t size)
{
    staging_regions.reserve_next(size);
}

void TransferChannelState::Detach() noexcept
{
    detached = true;
    staging_regions.clear();
}

bool TransferChannelState::IsDetached() const noexcept
{
    return detached;
}*/
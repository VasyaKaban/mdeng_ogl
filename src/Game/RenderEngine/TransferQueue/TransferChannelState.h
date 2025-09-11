#pragma once

#include <functional>
#include "hrs/rc.hpp"
#include "hrs/forward_pool.hpp"
#include "Core/Render/Render.h"
#include "../TaskTree/TaskState.hpp"

struct TransferChannelStateInfo
{
    std::size_t pool_block_size;
    std::size_t pool_blocks_reserve;
};

using TransferCallback = std::function<void()>;

struct TransferBufferOperation
{
    Render::Buffer* buffer;
    std::vector<Render::MemoryBufferCopyRegion> regions;
};

struct TransferImageOperation
{
    Render::Image* image;
    std::vector<Render::MemoryImageCopyRegion> regions;
};

struct TransferCallbackOperation
{
    TransferCallback cback;
};

struct TransferRegion
{
    std::variant<TransferBufferOperation, TransferImageOperation, TransferCallbackOperation> op;
};

class TransferChannelState : public hrs::rc
{
    friend class TransferChannel;
public:
    TransferChannelState(const TransferChannelStateInfo& info);
    ~TransferChannelState();

    void Transfer(TransferBufferOperation&& op);
    void Transfer(TransferImageOperation&& op);
    void Transfer(TransferCallbackOperation&& op);

    void Reserve(std::size_t size);

    void Detach() noexcept;
private:
    hrs::forward_pool<TransferRegion> staging_regions;
    bool detached;
};

static_assert(TaskState<TransferChannelState>);
#pragma once

#include <functional>
#include "hrs/forward_pool.hpp"
#include "../TaskTree/Task.h"
#include "TransferOperation.h"

class TransferQueue;

struct TransferChannelInfo
{
    std::size_t pool_block_size;
    std::size_t pool_blocks_reserve;
};

class TransferChannel : public TaskBase
{
public:
    TransferChannel(TransferQueue* _parent, TaskKey&& key, const TransferChannelInfo& info);
    virtual ~TransferChannel() override;

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) override;
    virtual void End(const EvaluateDesc& eval_desc) override;

    void Transfer(TransferBufferOperation&& op);
    void Transfer(TransferImageOperation&& op);
    void Transfer(TransferCallbackOperation&& op);

    void Reserve(std::size_t size);
private:
    hrs::forward_pool<TransferRegion> staging_regions;
};

CHECK_TASK_IS_READY(TransferChannel)
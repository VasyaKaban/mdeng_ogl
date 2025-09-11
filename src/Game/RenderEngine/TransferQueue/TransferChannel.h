#pragma once

#include "hrs/rc.hpp"
#include "../TaskTree/Task.h"
#include "TransferChannelState.h"

class TransferQueue;

class TransferChannel : public TaskBase
{
public:
    TransferChannel(Task<TransferQueue>* _parent,
                    TaskKey&& key,
                    const TransferChannelStateInfo& info);
    virtual ~TransferChannel() override;

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) override;
    virtual void End(const EvaluateDesc& eval_desc) override;

    hrs::rc_ptr<TransferChannelState> GetState() const noexcept;
private:
    TaskStateOwner<TransferChannelState> state;
};

CHECK_TASK_IS_READY(TransferChannel)
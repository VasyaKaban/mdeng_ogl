#pragma once

/*#include <functional>
#include "hrs/rc.hpp"
#include "hrs/forward_pool.hpp"
#include "Core/Render/Render.h"
#include "../TaskTree/TaskState.hpp"



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
    bool IsDetached() const noexcept;
private:
    hrs::forward_pool<TransferRegion> staging_regions;
    bool detached;
};*/

//static_assert(TaskState<TransferChannelState>);
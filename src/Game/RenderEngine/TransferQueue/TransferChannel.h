#pragma once

#include <functional>
#include "hrs/non_creatable.hpp"
#include "hrs/forward_pool.hpp"
#include "../TaskTree/Task.h"
#include "Core/Render/Render.h"

struct TransferChannelInfo
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

class TransferQueue;

class TransferChannel : public Task
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

static_assert(!std::is_abstract_v<TransferChannel>);
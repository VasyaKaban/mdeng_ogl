#pragma once

#include "hrs/non_creatable.hpp"
#include "hrs/forward_pool.hpp"
#include "Core/Render/Objects/Buffer/Buffer.h"
#include "Core/Render/Objects/Fence/Fence.h"
#include "../TaskTree/TaskTree.h"
#include "TransferRegion.h"

struct TransferChannelInfo
{
    std::uint32_t size;
    std::uint64_t regions_block_size;
    std::uint32_t regions_reserve;
};

class TransferQueue;
enum class Format;

class TransferChannel : hrs::non_copyable, hrs::non_movable, public Task
{
public:
    TransferChannel(TransferQueue* _parent, const TransferChannelInfo& info);
    virtual ~TransferChannel() override;

    virtual void Evaluate(EvaluateDesc& eval_desc) override;

    virtual Task* GetParent() noexcept override;
    virtual const Task* GetParent() const noexcept override;

    void Reserve(std::size_t size);
    void Transfer(std::span<const TransferRegion> regions);

    bool CanTransfer(GLintptr size) const noexcept;
    bool CanTransfer(Format format, const BufferImageCopyRegion& reg) const noexcept;
private:
    TransferQueue* parent;
    Fence* wait_on_fence;

    std::byte* mapped_ptr;
    Buffer buffer;

    GLintptr offset;
    hrs::forward_pool<TransferRegion> regions_pool;
};

static_assert(!std::is_abstract_v<TransferChannel>);
#pragma once

#include "Tasks/QueueTask.h"
#include "TransferQueue/TransferChannel.h"

class TransferQueue;
class RenderQueue;
class TransferChannel;

struct RenderEngineInfo
{
    std::uint16_t resource_set_count;
    TransferChannelInfo transfer_channel_info;
};

class RenderEngine : public Task
{
public:
    RenderEngine(const RenderEngineInfo& info, std::unique_ptr<Render::Context>&& _context);
    virtual ~RenderEngine() override;

    std::uint16_t GetResourceSetCount() const noexcept;
    std::uint16_t GetCurrentResourceSetIndex() const noexcept;
    std::uint16_t GetPreviousResourceSetIndex() const noexcept;
    std::uint16_t GetNextResourceSetIndex() const noexcept;

    void AcquireNextResourceSet() noexcept;
    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) override;
    virtual void End(const EvaluateDesc& eval_desc) override;

    TransferQueue* GetTransferQueue() const noexcept;
    RenderQueue* GetRenderQueue() const noexcept;
    TransferChannel* GetTransferChannel() const noexcept;
    Render::Context* GetContext() const noexcept;
private:
    std::unique_ptr<Render::Context> context;

    std::uint16_t resource_set_count;
    std::uint16_t current_resource_set_index;

    hrs::rc_ptr<TransferQueue> transfer_queue;
    hrs::rc_ptr<RenderQueue> render_queue;
    hrs::rc_ptr<TransferChannel> transfer_channel;
};
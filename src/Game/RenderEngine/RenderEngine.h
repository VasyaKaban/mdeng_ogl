#pragma once

#include "Tasks/QueueTask.h"
#include "TransferQueue/TransferChannel.h"
#include "TransferQueue/TransferQueue.h"
#include "RenderQueue/RenderQueue.h"

struct RenderEngineInfo
{
    std::uint16_t resource_set_count;
};

class RenderEngine : public TaskBase
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

    Task<TransferQueue>* GetTransferQueue() const noexcept;
    Task<RenderQueue>* GetRenderQueue() const noexcept;
    Render::Context* GetContext() const noexcept;
private:
    std::unique_ptr<Render::Context> context;

    std::uint16_t resource_set_count;
    std::uint16_t current_resource_set_index;

    Task<TransferQueue>* transfer_queue;
    Task<RenderQueue>* render_queue;
};
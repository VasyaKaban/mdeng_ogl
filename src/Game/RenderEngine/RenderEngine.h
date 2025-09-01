#pragma once

#include <map>
#include "hrs/non_creatable.hpp"
#include "TaskTree/TaskTree.h"
#include "Tasks/QueueTask.h"
#include "../ResourceManager/ResourceManager.h"
#include "TransferQueue/TransferChannel.h"

class Context;

class TransferQueue;
class RenderQueue;
class TransferChannel;

struct RenderEngineInfo
{
    std::uint16_t resource_set_count;

    TransferChannelInfo transfer_channel_info;
    ResourceManagerInfo resource_manager_info;
};

class RenderEngine : hrs::non_copyable, hrs::non_movable, public Task
{
    RenderEngine(const RenderEngineInfo& info, std::unique_ptr<Context>&& _context);
    void init_default_children(const RenderEngineInfo& info);
public:
    virtual ~RenderEngine() override;

    static RenderEngine* Init(const RenderEngineInfo& info, std::unique_ptr<Context>&& _context);
    static RenderEngine* GetInstance() noexcept;
    static void Close();

    std::uint16_t GetResourceSetCount() const noexcept;
    std::uint16_t GetCurrentResourceSetIndex() const noexcept;
    std::uint16_t GetPreviousResourceSetIndex() const noexcept;
    std::uint16_t GetNextResourceSetIndex() const noexcept;

    TaskTree::TaskHandle<> Insert(TaskTree::Priority priority,
                                  const HashedString& name,
                                  std::unique_ptr<QueueTask>&& task);

    TaskTree::TaskHandle<>
    Insert(TaskTree::Priority priority, HashedString&& name, std::unique_ptr<QueueTask>&& task);

    TaskTree::TaskHandle<> Insert(TaskTree::Priority priority,
                                  const HashedStringView& name,
                                  std::unique_ptr<QueueTask>&& task);

    void Erase(TaskTree::TaskHandle<> handle);
    std::optional<TaskTree::TaskHandle<>> Find(const HashedStringView& name) noexcept;

    void AcquireNextResourceSet() noexcept;
    virtual void Evaluate(EvaluateDesc& eval_desc) override;

    virtual Task* GetParent() noexcept override;
    virtual const Task* GetParent() const noexcept override;

    TransferQueue* GetTransferQueue() noexcept;
    const TransferQueue* GetTransferQueue() const noexcept;

    RenderQueue* GetRenderQueue() noexcept;
    const RenderQueue* GetRenderQueue() const noexcept;

    TransferChannel* GetTransferChannel() noexcept;
    const TransferChannel* GetTransferChannel() const noexcept;

    Context* GetContext() noexcept;
    const Context* GetContext() const noexcept;

    ResourceManager* GetResourceManager() noexcept;
    const ResourceManager* GetResourceManager() const noexcept;
private:
    static inline RenderEngine* instance = nullptr;

    std::unique_ptr<Context> context;
    TaskTree task_tree;

    std::uint16_t resource_set_count;
    std::uint16_t current_resource_set_index;

    TaskTree::TaskHandle<TransferQueue> transfer_queue_handle;
    TaskTree::TaskHandle<RenderQueue> render_queue_handle;
    TaskTree::TaskHandle<TransferChannel> transfer_channel_handle;

    ResourceManager resource_manager;
};
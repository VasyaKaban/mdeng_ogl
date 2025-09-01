#include "RenderEngine.h"
#include "TransferQueue/TransferQueue.h"
#include "RenderQueue/RenderQueue.h"
#include "TransferQueue/TransferChannel.h"
#include "Core/Render/Context/Context.h"
#include "Core/Window/GraphicWindow.h"
#include "Core/Window/WindowSubsystem.h"

RenderEngine::RenderEngine(const RenderEngineInfo& info, std::unique_ptr<Context>&& _context)
    : context(std::move(_context)),
      task_tree(),
      resource_set_count(info.resource_set_count),
      current_resource_set_index(0),
      resource_manager(info.resource_manager_info)
{}

void RenderEngine::init_default_children(const RenderEngineInfo& info)
{
    Queue transfer_queue = context->GetQueue(QueueSpecialization::Transfer);
    transfer_queue_handle = task_tree
                                .Insert(0,
                                        HashedStringView("TransferQueue"),
                                        std::unique_ptr<TransferQueue>(
                                            new TransferQueue(this, std::move(transfer_queue))))
                                .Rebind<TransferQueue>();

    {
        TransferQueue* transfer_queue = static_cast<TransferQueue*>(*transfer_queue_handle);
        transfer_channel_handle =
            transfer_queue
                ->Insert(0,
                         HashedStringView("TransferChannel"),
                         std::unique_ptr<TransferChannel>(
                             new TransferChannel(transfer_queue, info.transfer_channel_info)))
                .Rebind<TransferChannel>();
    }

    Queue render_queue = context->GetQueue(QueueSpecialization::Graphics);
    render_queue_handle =
        task_tree
            .Insert(1,
                    HashedStringView("RenderQueue"),
                    std::unique_ptr<RenderQueue>(new RenderQueue(this, std::move(render_queue))))
            .Rebind<RenderQueue>();
}

RenderEngine::~RenderEngine()
{}

RenderEngine* RenderEngine::Init(const RenderEngineInfo& info, std::unique_ptr<Context>&& _context)
{
    instance = new RenderEngine(info, std::move(_context));
    instance->init_default_children(info);
    return instance;
}

RenderEngine* RenderEngine::GetInstance() noexcept
{
    return instance;
}

void RenderEngine::Close()
{
    delete instance;
}

std::uint16_t RenderEngine::GetResourceSetCount() const noexcept
{
    return resource_set_count;
}

std::uint16_t RenderEngine::GetCurrentResourceSetIndex() const noexcept
{
    return current_resource_set_index;
}

std::uint16_t RenderEngine::GetPreviousResourceSetIndex() const noexcept
{
    if(current_resource_set_index == 0)
        return resource_set_count - 1;

    return (current_resource_set_index - 1) % resource_set_count;
}

std::uint16_t RenderEngine::GetNextResourceSetIndex() const noexcept
{
    return (current_resource_set_index + 1) % resource_set_count;
}

TaskTree::TaskHandle<> RenderEngine::Insert(TaskTree::Priority priority,
                                            const HashedString& name,
                                            std::unique_ptr<QueueTask>&& task)
{
    return task_tree.Insert(priority, name, std::move(task));
}

TaskTree::TaskHandle<> RenderEngine::Insert(TaskTree::Priority priority,
                                            HashedString&& name,
                                            std::unique_ptr<QueueTask>&& task)
{
    return task_tree.Insert(priority, std::move(name), std::move(task));
}

TaskTree::TaskHandle<> RenderEngine::Insert(TaskTree::Priority priority,
                                            const HashedStringView& name,
                                            std::unique_ptr<QueueTask>&& task)
{
    return task_tree.Insert(priority, name, std::move(task));
}

void RenderEngine::Erase(TaskTree::TaskHandle<> handle)
{
    task_tree.Erase(handle);
}

std::optional<TaskTree::TaskHandle<>> RenderEngine::Find(const HashedStringView& name) noexcept
{
    return task_tree.Find(name);
}

void RenderEngine::AcquireNextResourceSet() noexcept
{
    current_resource_set_index = (current_resource_set_index + 1) % resource_set_count;
}

void RenderEngine::Evaluate([[maybe_unused]] EvaluateDesc& eval_desc)
{
    task_tree.Evaluate(eval_desc);
}

Task* RenderEngine::GetParent() noexcept
{
    return nullptr;
}

const Task* RenderEngine::GetParent() const noexcept
{
    return nullptr;
}

TransferQueue* RenderEngine::GetTransferQueue() noexcept
{
    return *transfer_queue_handle;
}

const TransferQueue* RenderEngine::GetTransferQueue() const noexcept
{
    return *transfer_queue_handle;
}

RenderQueue* RenderEngine::GetRenderQueue() noexcept
{
    return *render_queue_handle;
}

const RenderQueue* RenderEngine::GetRenderQueue() const noexcept
{
    return *render_queue_handle;
}

TransferChannel* RenderEngine::GetTransferChannel() noexcept
{
    return *transfer_channel_handle;
}

const TransferChannel* RenderEngine::GetTransferChannel() const noexcept
{
    return *transfer_channel_handle;
}

Context* RenderEngine::GetContext() noexcept
{
    return context.get();
}

const Context* RenderEngine::GetContext() const noexcept
{
    return context.get();
}

ResourceManager* RenderEngine::GetResourceManager() noexcept
{
    return &resource_manager;
}

const ResourceManager* RenderEngine::GetResourceManager() const noexcept
{
    return &resource_manager;
}
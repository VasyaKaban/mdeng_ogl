#include "TransferStorage.h"
#include "../RenderEngine.h"
#include "Core/Render/Context.h"

TransferStorage::TransferStorage(RenderEngine* _parent)
    : parent(_parent),
      queue(parent->GetContext()->GetQueue(Render::QueueSpecialization::Transfer)),
      command_pool(
          parent->GetContext()->CreateCommandPoolUnique(Render::CommandPoolInfo{.queue = queue})),
      command_buffer(command_pool->AllocateUnique()),
      write_started(false)
{}

TransferStorage::~TransferStorage()
{
    Flush();
}

void TransferStorage::Transfer(TransferBufferOperation&& op)
{
    start_write();

    op.buffer->Update(command_buffer.get(), op.regions);
}

void TransferStorage::Transfer(TransferImageOperation&& op)
{
    start_write();

    op.image->Update(command_buffer.get(), op.regions);
}

void TransferStorage::Transfer(TransferCallbackOperation&& op)
{
    start_write();

    op.cback();
}

void TransferStorage::Reserve(std::size_t size)
{
    //noop
}

void TransferStorage::Flush()
{
    if(!write_started)
        return;

    command_buffer->End();

    Render::CommandBuffer* cmd = command_buffer.get();
    const Render::QueueFlushInfo info = {.signal_fence = nullptr,
                                         .signal_seamphores = {},
                                         .command_buffers = {&cmd, 1}};
    queue->Flush(info);
    queue->WaitIdle();
    write_started = false;
}

void TransferStorage::start_write()
{
    if(!write_started)
    {
        const Render::QueueBeginInfo info = {.wait_seamphores = {}};
        queue->Begin(info);
        command_buffer->Begin();
    }
}
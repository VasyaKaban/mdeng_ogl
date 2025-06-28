#include "Context.h"
#include <stdexcept>
#include "../../Window/GraphicWindow.h"
#include "../Objects/Buffer/Buffer.h"
#include "../Objects/Queue/Queue.h"
#include "../Objects/Semaphore/Semaphore.h"

Context::Context(GraphicWindow* _parent, const ContextInfo& info, GLADloadfunc get_proc_addr)
    : parent(_parent),
      resource_set_count(info.resource_set_count),
      current_resource_set_index(0)
{
    int glad_ver = gladLoadGLContext(&loader, get_proc_addr);
    if(glad_ver == 0)
        throw std::runtime_error("Failed to load GLAD");
}

Context::~Context()
{
    WaitIdle();
}

void Context::MakeCurrent() noexcept
{
    parent->MakeContextCurrent();
}

Queue Context::GetQueue([[maybe_unused]] QueueSpecialization spec)
{
    //just return 'empty' queue -> we don't have a 'Queue' concept in opengl! Only implicit queue exists
    return Queue(this);
}

void Context::AcquireNextResourceSet()
{
    current_resource_set_index = (current_resource_set_index + 1) % resource_set_count;
}

void Context::ReleaseCurrentResourceSet()
{
    //noop
}

void Context::WaitIdle() noexcept
{
    loader.Finish();
}

void Context::AcquireNextSwapchainImage(Semaphore* signal_semaphore) //OGL -> noop
{
    parent->AcquireNextSwapchainImage();
    signal_semaphore->Set();
}

Framebuffer* Context::GetCurrentDefaultFramebuffer() noexcept
{
    return &default_framebuffer;
}

void Context::ReleaseSwapchainImage(PresentInfo& info) //SDL_SwapWindow();
{
    for(auto& sem: info.wait_semaphores)
        sem->Wait();

    parent->ReleaseSwapchainImage();
}

GraphicWindow* Context::GetWindow() noexcept
{
    return parent;
}

const GraphicWindow* Context::GetWindow() const noexcept
{
    return parent;
}

const GladGLContext& Context::GetLoader() const noexcept
{
    return loader;
}

std::uint16_t Context::GetResourceSetCount() const noexcept
{
    return resource_set_count;
}

std::uint16_t Context::GetCurrentResourceSetIndex() const noexcept
{
    return current_resource_set_index;
}

std::uint16_t Context::GetPreviousResourceSetIndex() const noexcept
{
    if(current_resource_set_index == 0)
        return resource_set_count - 1;

    return (current_resource_set_index - 1) % resource_set_count;
}

std::uint16_t Context::GetNextResourceSetIndex() const noexcept
{
    return (current_resource_set_index + 1) % resource_set_count;
}

void Context::SetMemoryBarrier(MemoryBarrierFlags flags, bool by_region) const noexcept
{
    if(!by_region)
        loader.MemoryBarrier(static_cast<GLbitfield>(flags));
    else
        loader.MemoryBarrierByRegion(static_cast<GLbitfield>(flags));
}
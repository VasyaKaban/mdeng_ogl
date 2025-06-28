#pragma once

#include <vector>
#include <string>
#include "../../hrs/non_creatable.hpp"
#include "../Common.h"
#include "../Objects/Queue/Queue.h"
#include "../Objects/Framebuffer/Framebuffer.h"

class GraphicWindow;

struct ContextInfo
{
    std::uint16_t resource_set_count;
};

enum class QueueSpecialization
{
    Graphics,
    Compute,
    Transfer
};

enum MemoryBarrierFlagBits
{
    VertexAttribArrayBarrier = GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT,
    ElementArrayBarrier = GL_ELEMENT_ARRAY_BARRIER_BIT,
    UniformBarrier = GL_UNIFORM_BARRIER_BIT,
    TextureFetchBarrier = GL_TEXTURE_FETCH_BARRIER_BIT,
    ShaderImageAccessBarrier = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT,
    CommandBarrier = GL_COMMAND_BARRIER_BIT,
    PixelBufferBarrier = GL_PIXEL_BUFFER_BARRIER_BIT,
    TextureUpdateBarrier = GL_TEXTURE_UPDATE_BARRIER_BIT,
    QueyBufferbarrier = GL_QUERY_BUFFER_BARRIER_BIT,
    BufferUpdateBarrier = GL_BUFFER_UPDATE_BARRIER_BIT,
    FramebufferBarrier = GL_FRAMEBUFFER_BARRIER_BIT,
    TransformFeedbackBarrier = GL_TRANSFORM_FEEDBACK_BARRIER_BIT,
    AtomicCounterBarrier = GL_ATOMIC_COUNTER_BARRIER_BIT,
    ShaderStorageBarrier = GL_SHADER_STORAGE_BARRIER_BIT,
    ClientMappedBufferBarrier = GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT,
    AllBarrier = GL_ALL_BARRIER_BITS
};

using MemoryBarrierFlags = std::underlying_type_t<MemoryBarrierFlagBits>;

class Framebuffer;

struct PresentInfo
{
    std::span<Semaphore*> wait_semaphores;
};

class Context : hrs::non_copyable, hrs::non_movable
{
    friend class GraphicWindow;
    Context(GraphicWindow* _parent, const ContextInfo& info, GLADloadfunc get_proc_addr);
public:
    ~Context();

    void MakeCurrent() noexcept;

    Queue GetQueue(QueueSpecialization spec);

    void AcquireNextResourceSet();
    void ReleaseCurrentResourceSet();

    void WaitIdle() noexcept;

    void AcquireNextSwapchainImage(Semaphore* signal_semaphore); //OGL -> noop

    Framebuffer* GetCurrentDefaultFramebuffer() noexcept;

    void ReleaseSwapchainImage(PresentInfo& info); //SDL_SwapWindow();

    GraphicWindow* GetWindow() noexcept;
    const GraphicWindow* GetWindow() const noexcept;

    const GladGLContext& GetLoader() const noexcept;

    std::uint16_t GetResourceSetCount() const noexcept;
    std::uint16_t GetCurrentResourceSetIndex() const noexcept;
    std::uint16_t GetPreviousResourceSetIndex() const noexcept;
    std::uint16_t GetNextResourceSetIndex() const noexcept;

    //Common commands
    void SetMemoryBarrier(MemoryBarrierFlags flags, bool by_region) const noexcept;
private:
    GraphicWindow* parent;
    GladGLContext loader;

    Framebuffer default_framebuffer;

    std::uint16_t resource_set_count;
    std::uint16_t current_resource_set_index;
};
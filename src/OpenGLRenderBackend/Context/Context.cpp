#include "Context.h"
#include "../Objects/Queue/Queue.h"
#include "../Objects/Semaphore/Semaphore.h"
#include "../Objects/Buffer/Buffer.h"
#include "../Objects/CommandPool/CommandPool.h"
#include "../Objects/Fence/Fence.h"
#include "../Objects/Framebuffer/Framebuffer.h"
#include "../Objects/Image/Image.h"
#include "../Objects/ImageView/ImageView.h"
#include "../Objects/Pipeline/Pipeline.h"
#include "../Objects/RenderPass/RenderPass.h"
#include "../Objects/Sampler/Sampler.h"
#include "../Objects/Shader/Shader.h"
#include <stdexcept>

namespace OpenGL
{
    Context::Context(OpenGLBackend* _parent)
        : parent(_parent),
          default_framebuffer(this)
    {
        int glad_ver =
            gladLoadGLContext(&loader, reinterpret_cast<GLADloadfunc>(_parent->GetResolver()));
        if(glad_ver == 0)
            throw std::runtime_error("Failed to load GLAD");

        loader.Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS); //use only seamless cubemaps
        loader.ClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE); //make viewport like in D3D or VK
        loader.Enable(GL_PROGRAM_POINT_SIZE); //activate GLSL gl_PointSize
    }

    Context::~Context()
    {
        WaitIdle();
    }

    Render::Queue* Context::GetQueue([[maybe_unused]] Render::QueueSpecialization spec)
    {
        //just return 'empty' queue -> we don't have a 'Queue' concept in opengl! Only implicit queue exists
        return new Queue(this);
    }

    void Context::WaitIdle() noexcept
    {
        loader.Finish();
    }

    void Context::AcquireNextSwapchainImage(Render::Semaphore* signal_semaphore) //OGL -> noop
    {
        parent->AcquireNextSwapchainImage();
        static_cast<Semaphore*>(signal_semaphore)->Set();
    }

    Render::Framebuffer* Context::GetCurrentDefaultFramebuffer() noexcept
    {
        return &default_framebuffer;
    }

    void Context::ReleaseSwapchainImage(Render::PresentInfo& info) //SDL_SwapWindow();
    {
        for(auto& sem: info.wait_semaphores)
            static_cast<Semaphore*>(sem)->Wait();

        parent->ReleaseSwapchainImage();
    }

    RenderBackend* Context::GetBackend() const noexcept
    {
        return parent;
    }

    Render::Buffer* Context::CreateBuffer(const Render::BufferInfo& info)
    {
        return new Buffer(this, info);
    }

    Render::CommandPool* Context::CreateCommandPool(const Render::CommandPoolInfo& info)
    {
        return new CommandPool(this, info);
    }

    Render::Fence* Context::CreateFence()
    {
        return new Fence(this);
    }

    Render::Framebuffer* Context::CreateFramebuffer(const Render::FramebufferInfo& info)
    {
        return new Framebuffer(this, info);
    }

    Render::Image* Context::CreateImage(const Render::ImageInfo& info)
    {
        return new Image(this, info);
    }

    Render::ImageView* Context::CreateImageView(const Render::ImageViewInfo& info)
    {
        return new ImageView(this, info);
    }

    Render::Pipeline* Context::CreatePipeline(const Render::GraphicsPipelineInfo& info)
    {
        return new Pipeline(this, info);
    }

    Render::Pipeline* Context::CreatePipeline(const Render::ComputePipelineInfo& info)
    {
        return new Pipeline(this, info);
    }

    Render::RenderPass* Context::CreateRenderPass(const Render::RenderPassInfo& info)
    {
        return new RenderPass(this, info);
    }

    Render::Sampler* Context::CreateSampler(const Render::SamplerInfo& info)
    {
        return new Sampler(this, info);
    }

    Render::Semaphore* Context::CreateSemaphore()
    {
        return new Semaphore(this);
    }

    Render::Shader* Context::CreateShader(const Render::ShaderInfo& info)
    {
        return new Shader(this, info);
    }

    void Context::MakeCurrent() noexcept
    {
        parent->MakeContextCurrent();
    }

    const GladGLContext& Context::GetLoader() const noexcept
    {
        return loader;
    }

};
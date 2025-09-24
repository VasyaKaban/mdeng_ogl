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
    static void GLAPIENTRY debug_messenger_callback(GLenum source,
                                                    GLenum type,
                                                    GLuint id,
                                                    GLenum severity,
                                                    GLsizei length,
                                                    const GLchar* message,
                                                    const void* userParam)
    {
        (*reinterpret_cast<const std::function<Render::DebugMessengerCallback>*>(userParam))(
            NativeDebugMessengerSeverityFlagBitToSpec(severity),
            NativeDebugMessengerTypeFlagBitToSpec(type),
            id,
            {reinterpret_cast<const char*>(message), static_cast<std::size_t>(length)});
    }

    Context::Context(OpenGLBackend* _parent)
        : parent(_parent),
          default_framebuffer(this),
          default_queue(this)
    {
        int glad_ver =
            gladLoadGLContext(&loader, reinterpret_cast<GLADloadfunc>(_parent->GetResolver()));
        if(glad_ver == 0)
            throw std::runtime_error("Failed to load GLAD");

        loader.Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS); //use only seamless cubemaps
        loader.ClipControl(/*GL_UPPER_LEFT*/ GL_LOWER_LEFT,
                           GL_ZERO_TO_ONE); //make viewport like in D3D or VK(only depth)
        loader.Enable(GL_PROGRAM_POINT_SIZE); //activate GLSL gl_PointSize

        GLint extensions_number = 0;
        loader.GetIntegerv(GL_NUM_EXTENSIONS, &extensions_number);

        extensions.resize(extensions_number);
        for(std::size_t i = 0; i < extensions_number; i++)
            extensions[i] = reinterpret_cast<const char*>(loader.GetStringi(GL_EXTENSIONS, i));

        properties.vendor_name = reinterpret_cast<const char*>(loader.GetString(GL_VENDOR));
        properties.device_name = reinterpret_cast<const char*>(loader.GetString(GL_RENDERER));
        properties.extensions = extensions;
    }

    Context::~Context()
    {
        WaitIdle();
    }

    Render::ContextProperties Context::GetProperties() const
    {
        return properties;
    }

    Render::Queue* Context::GetQueue([[maybe_unused]] Render::QueueSpecialization spec)
    {
        //just return 'empty' queue -> we don't have a 'Queue' concept in opengl! Only implicit queue exists
        return &default_queue;
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

    void Context::ReleaseSwapchainImage(const Render::PresentInfo& info) //SDL_SwapWindow();
    {
        for(auto& sem: info.wait_semaphores)
            static_cast<Semaphore*>(sem)->Wait();

        parent->ReleaseSwapchainImage();
    }

    void Context::SetDebugMessenger(const Render::DebugMessengerInfo& info)
    {
        GLint context_flags = 0;
        loader.GetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
        if(!(context_flags & GL_CONTEXT_FLAG_DEBUG_BIT))
            throw std::runtime_error("Context is not in debug mode");

        auto filter_types = DebugMessengerTypeFlagsToNativeInverted(info.types);
        auto filter_severities = DebugMessengerSeverityFlagsToNativeInverted(info.severities);

        loader.DebugMessageControl(GL_DONT_CARE,
                                   GL_DONT_CARE,
                                   GL_DONT_CARE,
                                   0,
                                   nullptr,
                                   GL_TRUE); //drop all filters

        for(std::size_t i = 0; i < filter_types.size; i++)
            loader.DebugMessageControl(GL_DONT_CARE,
                                       filter_types.data[i],
                                       GL_DONT_CARE,
                                       0,
                                       nullptr,
                                       GL_FALSE);

        for(std::size_t i = 0; i < filter_severities.size; i++)
            loader.DebugMessageControl(GL_DONT_CARE,
                                       GL_DONT_CARE,
                                       filter_severities.data[i],
                                       0,
                                       nullptr,
                                       GL_FALSE);

        loader.Enable(GL_DEBUG_OUTPUT);

#ifndef NDEBUG
        loader.Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

        debug_callback = info.callback;

        loader.DebugMessageCallback(debug_messenger_callback, &debug_callback);
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
#include "Context.h"
#include "../Objects/Queue/Queue.h"
#include "../Objects/Semaphore/Semaphore.h"
#include "../Objects/Buffer/Buffer.h"
#include "../Objects/CommandPool/CommandPool.h"
#include "../Objects/DescriptorPool/DescriptorPool.h"
#include "../Objects/DescriptorSetLayout/DescriptorSetLayout.h"
#include "../Objects/Fence/Fence.h"
#include "../Objects/Framebuffer/Framebuffer.h"
#include "../Objects/Image/Image.h"
#include "../Objects/ImageView/ImageView.h"
#include "../Objects/Pipeline/Pipeline.h"
#include "../Objects/RenderPass/RenderPass.h"
#include "../Objects/Sampler/Sampler.h"
#include "../Objects/Shader/Shader.h"
#include <stdexcept>
#include <format>

namespace OpenGL
{
    static std::vector<Render::MemoryType> create_memory_types()
    {
        constexpr std::size_t MEMORY_TYPES_COUNT =
            (Render::MemoryTypePropertyFlagBits::HostCached << 1) - 1;

        std::vector<Render::MemoryType> types;
        types.reserve(MEMORY_TYPES_COUNT);
        for(std::size_t i = 0; i < MEMORY_TYPES_COUNT; i++)
            types.push_back(Render::MemoryType{
                .memory_heap_flags = Render::MemoryHeapFlagBits::DeviceLocalHeap,
                .memory_type_flags = static_cast<Render::MemoryTypePropertyFlags>(i)});

        return types;
    }

    constexpr static std::string_view REQUIRED_EXTENSIONS[] = {"GL_EXT_texture_compression_s3tc",
                                                               "GL_EXT_texture_sRGB"};

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

        std::vector<std::string> extensions;

        extensions.resize(extensions_number);
        for(std::size_t i = 0; i < extensions_number; i++)
            extensions[i] = reinterpret_cast<const char*>(loader.GetStringi(GL_EXTENSIONS, i));

        std::string unsupported_extensions;
        for(const auto& req_ext: REQUIRED_EXTENSIONS)
        {
            auto it = std::ranges::find(extensions, req_ext);
            if(it == extensions.end())
            {
                if(unsupported_extensions.empty())
                    unsupported_extensions = req_ext;
                else
                    unsupported_extensions += std::format("\n{}", req_ext);
            }
        }

        if(!unsupported_extensions.empty())
            throw std::runtime_error(
                std::format("Required extensions are not supported:\n{}", unsupported_extensions));

        GLint major = 0;
        GLint minor = 0;
        loader.GetIntegerv(GL_MAJOR_VERSION, &major);
        loader.GetIntegerv(GL_MINOR_VERSION, &minor);

        properties.context_name = "OpenGL";
        properties.supported_backend_type = RenderBackendType::OpenGL;
        properties.version = Render::MakeVersion(major, minor);
        properties.vendor_name = reinterpret_cast<const char*>(loader.GetString(GL_VENDOR));
        properties.device_name = reinterpret_cast<const char*>(loader.GetString(GL_RENDERER));
        properties.extensions = std::move(extensions);
        properties.supported_syntax = Render::ShaderSyntaxFlagBits::GLSL;
        properties.queue_family_properties.push_back(Render::QueueFamilyProperties{
            .specialization = Render::QueueSpecializationFlagBits::TransferSpec |
                              Render::QueueSpecializationFlagBits::ComputeSpec |
                              Render::QueueSpecializationFlagBits::GraphicsSpec |
                              Render::QueueSpecializationFlagBits::PresentSpec,
            .queue_count = 1});
        properties.memory_types = create_memory_types();
        properties.command_buffer_strategy = Render::CommandBufferStrategy::Immediate;
    }

    Context::~Context()
    {
        WaitIdle();
    }

    const Render::ContextProperties& Context::GetProperties() const
    {
        return properties;
    }

    Render::Queue* Context::GetQueue(std::uint32_t queue_family_index, std::uint32_t index)
    {
        if(!(queue_family_index == 0 && index == 0))
            throw std::runtime_error("Bad queue family index or queue index");

        //just return 'empty' queue -> we don't have a 'Queue' concept in opengl! Only implicit queue exists
        return &default_queue;
    }

    void Context::WaitIdle() noexcept
    {
        loader.Finish();
    }

    bool Context::AcquireNextSwapchainImage(
        Render::Semaphore*
            signal_semaphore) //OGL -> noop; false -> should recreate window/swapchain
    {
        parent->AcquireNextSwapchainImage();
        static_cast<Semaphore*>(signal_semaphore)->Set();

        return true;
    }

    Render::Image* Context::
        GetCurrentSwapchainImageHandle() //returns swapchain current image; OGL -> return impl-defined address that OpenGL impl will convert to inner calls; VK -> return already created image from swapchain
    {
        return reinterpret_cast<Render::Image*>(
            this); //we just cast context to make non existent pointer to swapchain image. Not for use. Just a hint for pipeline barrier
    }

    Render::Framebuffer* Context::GetCurrentDefaultFramebuffer() noexcept
    {
        return &default_framebuffer;
    }

    bool Context::ReleaseSwapchainImage(const Render::PresentInfo& info) //SDL_SwapWindow();
    {
        for(auto& sem: info.wait_semaphores)
            static_cast<Semaphore*>(sem)->Wait();

        parent->ReleaseSwapchainImage();

        return true;
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

    Render::Buffer*
    Context::CreateBuffer(const Render::BufferInfo& info,
                          std::span<const std::uint32_t> desired_memory_type_indices)
    {
        return new Buffer(this, info, desired_memory_type_indices);
    }

    void Context::CreateBuffersBatch(std::span<const Render::BufferInfo> infos,
                                     std::span<const std::uint32_t> desired_memory_type_indices,
                                     std::span<Render::Buffer*> buffers)
    {
        std::size_t i = 0;
        try
        {
            for(; i < infos.size(); i++)
                buffers[i] = CreateBuffer(infos[i], desired_memory_type_indices);
        }
        catch(...)
        {
            for(std::size_t j = 0; j < i; j++)
                delete buffers[j];

            throw;
        }
    }

    Render::CommandPool* Context::CreateCommandPool(const Render::CommandPoolInfo& info)
    {
        return new CommandPool(this, info);
    }

    Render::DescriptorPool* Context::CreateDescriptorPool(const Render::DescriptorPoolInfo& info)
    {
        return new DescriptorPool(this, info);
    }

    Render::DescriptorSetLayout*
    Context::CreateDescriptorSetLayout(const Render::DescriptorSetLayoutInfo& info)
    {
        return new DescriptorSetLayout(this, info);
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

    void Context::CreateImagesBatch(std::span<const Render::ImageInfo> infos,
                                    std::span<Render::Image*> images)
    {
        std::size_t i = 0;
        try
        {
            for(; i < infos.size(); i++)
                images[i] = CreateImage(infos[i]);
        }
        catch(...)
        {
            for(std::size_t j = 0; j < i; j++)
                delete images[j];

            throw;
        }
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
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
        constexpr std::size_t MAX_MEMORY_TYPES_COUNT =
            (Render::MemoryTypePropertyFlagBits::HostCached << 1) - 1;

        std::vector<Render::MemoryType> types;
        types.reserve(MAX_MEMORY_TYPES_COUNT);
        for(std::size_t i = 0; i < MAX_MEMORY_TYPES_COUNT; i++)
        {
            if((i & Render::MemoryTypePropertyFlagBits::HostCoherent ||
                i & Render::MemoryTypePropertyFlagBits::HostCached) &&
               !(i & Render::MemoryTypePropertyFlagBits::HostMappingReadable ||
                 i & Render::MemoryTypePropertyFlagBits::HostMappingWritable))
            {
                continue;
            }

            types.push_back(Render::MemoryType{
                .memory_heap_flags = Render::MemoryHeapFlagBits::DeviceLocalHeap,
                .memory_type_flags = static_cast<Render::MemoryTypePropertyFlags>(i)});
        }

        return types;
    }

    //constexpr static std::array REQUIRED_EXTENSIONS = {OPENGL_REQUIRED_EXTENSIONS};

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

    Context::Context(Core::OpenGLBackend* _parent)
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

        /*std::string unsupported_extensions;
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
                std::format("Required extensions are not supported:\n{}", unsupported_extensions));*/

        GLint major = 0;
        GLint minor = 0;
        loader.GetIntegerv(GL_MAJOR_VERSION, &major);
        loader.GetIntegerv(GL_MINOR_VERSION, &minor);

        properties.context_name = "OpenGL";
        properties.supported_backend_type = Core::RenderBackendType::OpenGL;
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
        properties.persistent_mapping_used = true;
    }

    Context::~Context()
    {
        WaitIdle();
    }

    const Render::ContextProperties& Context::GetProperties() const
    {
        return properties;
    }

    std::optional<Render::BufferFormatProperties>
    Context::GetBufferFormatProperties(const Render::BufferFormatInfo& info) const
    {
        if(IsFormatSupportedAsVertexInput(info.format))
        {
            return Render::BufferFormatProperties{
                .features = Render::FormatFeatureFlagBits::FormatFeatureVertexBufferBit};
        }

        return std::nullopt;
    }

    std::optional<Render::ImageFormatProperties>
    Context::GetImageFormatProperties(const Render::ImageFormatInfo& info) const
    {
        /*struct ImageFormatInfo
    {
        Format format;
        ImageType type;
        bool sampled;
        ImageUsageFlags usage;
        ImageFlags flags;
    };

    struct ImageFormatProperties
    {
        Extent3D max_extent;
        std::uint32_t max_mip_levels;
        std::uint32_t max_array_layers;
        SampleCountFlags sample_count;
        FormatFeatureFlags features;
    };*/

        GLenum inner_type = DecodeImageType(info.type, info.layered, info.sampled);
        GLenum native_format = FormatToNative(info.format);

        GLint supported = GL_FALSE;
        //do not check extension formats -< we just init is with GL_FALSE so on error we still get false
        loader.GetInternalformativ(inner_type,
                                   native_format,
                                   GL_INTERNALFORMAT_SUPPORTED,
                                   1,
                                   &supported);
        if(supported == GL_FALSE)
            return std::nullopt;

        GLint width;
        GLint height;
        GLint depth;
        loader.GetInternalformativ(inner_type, native_format, GL_MAX_WIDTH, 1, &width);

        if(info.type == Render::ImageType::Image1D)
        {
            height = 1;
            width = 1;
        }
        else
        {
            loader.GetInternalformativ(inner_type, native_format, GL_MAX_HEIGHT, 1, &height);
            if(info.type == Render::ImageType::Image2D)
                depth = 1;
            else
                loader.GetInternalformativ(inner_type, native_format, GL_MAX_DEPTH, 1, &depth);
        }

        GLint support_mipmaps;
        loader.GetInternalformativ(inner_type, native_format, GL_MIPMAP, 1, &support_mipmaps);

        GLint max_mip_levels;
        if(!support_mipmaps)
            max_mip_levels = 1;
        else
            max_mip_levels = 1000; //in OpenGL spec it is a default value -> so do not care...

        GLint max_array_layers;
        if(info.type == Render::ImageType::Image3D && !info.layered)
            max_array_layers = 1;
        else
            loader.GetInternalformativ(inner_type,
                                       native_format,
                                       GL_MAX_LAYERS,
                                       1,
                                       &max_array_layers);

        static std::vector<GLint> samples(std::popcount(
            (static_cast<std::uint64_t>(Render::SampleCount::SampleCount_64) << 1) - 1));

        Render::SampleCountFlags sample_count = {};
        if(!info.sampled)
            sample_count = Render::SampleCount::SampleCount_1;
        else
        {
            GLint num_samples;
            loader.GetInternalformativ(inner_type,
                                       native_format,
                                       GL_NUM_SAMPLE_COUNTS,
                                       1,
                                       &num_samples);
            if(num_samples > samples.size())
                samples.resize(num_samples);

            loader.GetInternalformativ(inner_type,
                                       native_format,
                                       GL_SAMPLES,
                                       num_samples,
                                       samples.data());

            for(std::size_t i = 0; i < num_samples; i++)
            {
                if(samples[i] <= Render::SampleCount::SampleCount_64 &&
                   std::popcount(static_cast<std::uint32_t>(samples[i])) == 1) //is popwer of two
                {
                    sample_count |= samples[i];
                }
            }
        }

        Render::FormatFeatureFlags features = {};

        //FormatFeatureSampledImageBit; -> all???
        //FormatFeatureStorageImageBit; -> set
        //FormatFeatureStorageImageAtomicBit; -> set
        //FormatFeatureVertexBufferBit; -> none
        //FormatFeatureColorAttachmentBit; -> query
        //FormatFeatureColorAttachmentBlendBit; -> query
        //FormatFeatureDepthStencilAttachmentBit; -> query
        //FormatFeaturesampledImageFilterLinearBit; -> query

        GLint texture_view_support;
        loader.GetInternalformativ(inner_type,
                                   native_format,
                                   GL_TEXTURE_VIEW,
                                   1,
                                   &texture_view_support);
        if(texture_view_support != GL_NONE)
        {
            features |= Render::FormatFeatureSampledImageBit;

            switch(info.format)
            {
                case Render::Format::R32G32B32A32_FLOAT:
                case Render::Format::R16G16B16A16_FLOAT:
                case Render::Format::R32G32_FLOAT:
                case Render::Format::R16G16_FLOAT:
                case Render::Format::R11G11B10_FLOAT:
                case Render::Format::R32_FLOAT:
                case Render::Format::R16_FLOAT:

                case Render::Format::R32G32B32A32_UINT:
                case Render::Format::R16G16B16A16_UINT:
                case Render::Format::R10G10B10A2_UINT:
                case Render::Format::R8G8B8A8_UINT:
                case Render::Format::R32G32_UINT:
                case Render::Format::R16G16_UINT:
                case Render::Format::R8G8_UINT:
                case Render::Format::R32_UINT:
                case Render::Format::R16_UINT:
                case Render::Format::R8_UINT:

                case Render::Format::R32G32B32A32_SINT:
                case Render::Format::R16G16B16A16_SINT:
                case Render::Format::R8G8B8A8_SINT:
                case Render::Format::R32G32_SINT:
                case Render::Format::R16G16_SINT:
                case Render::Format::R8G8_SINT:
                case Render::Format::R32_SINT:
                case Render::Format::R16_SINT:
                case Render::Format::R8_SINT:

                case Render::Format::R16G16B16A16_UNORM:
                case Render::Format::R10G10B10A2_UNORM:
                case Render::Format::R8G8B8A8_UNORM:
                case Render::Format::R16G16_UNORM:
                case Render::Format::R8G8_UNORM:
                case Render::Format::R16_UNORM:
                case Render::Format::R8_UNORM:

                case Render::Format::R16G16B16A16_SNORM:
                case Render::Format::R8G8B8A8_SNORM:
                case Render::Format::R16G16_SNORM:
                case Render::Format::R8G8_SNORM:
                case Render::Format::R16_SNORM:
                case Render::Format::R8_SNORM:
                    features |= Render::FormatFeatureStorageImageBit;
                    break;
                default:
                    break;
            }

            if(info.format == Render::Format::R32_SINT || info.format == Render::Format::R32_UINT)
                features |= Render::FormatFeatureStorageImageAtomicBit;

            GLint color_renderable;
            GLint depth_renderable;
            GLint stencil_renderable;
            loader.GetInternalformativ(inner_type,
                                       native_format,
                                       GL_COLOR_RENDERABLE,
                                       1,
                                       &color_renderable);

            loader.GetInternalformativ(inner_type,
                                       native_format,
                                       GL_DEPTH_RENDERABLE,
                                       1,
                                       &depth_renderable);

            loader.GetInternalformativ(inner_type,
                                       native_format,
                                       GL_STENCIL_RENDERABLE,
                                       1,
                                       &stencil_renderable);

            if(color_renderable == GL_TRUE)
                features |= Render::FormatFeatureColorAttachmentBit;

            if(depth_renderable == GL_TRUE || stencil_renderable == GL_TRUE)
                features |= Render::FormatFeatureDepthStencilAttachmentBit;

            GLint color_blend;
            loader.GetInternalformativ(inner_type,
                                       native_format,
                                       GL_FRAMEBUFFER_BLEND,
                                       1,
                                       &color_blend);
            if(color_blend == GL_TRUE)
                features |= Render::FormatFeatureColorAttachmentBlendBit;

            GLint linear_filter;
            loader.GetInternalformativ(inner_type, native_format, GL_FILTER, 1, &linear_filter);
            if(linear_filter == GL_TRUE)
                features |= Render::FormatFeatureSampledImageFilterLinearBit;
        }

        return Render::ImageFormatProperties{
            .max_extent = {.width = static_cast<std::uint32_t>(width),
                           .height = static_cast<std::uint32_t>(height),
                           .depth = static_cast<std::uint32_t>(depth)},
            .max_mip_levels = static_cast<std::uint32_t>(max_mip_levels),
            .max_array_layers = static_cast<std::uint32_t>(max_array_layers),
            .sample_count = sample_count,
            .features = features};
    }

    Render::Queue* Context::GetQueue(const Render::QueueInfo& info)
    {
        if(!(info.family_index == 0 && info.index == 0))
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

    Core::RenderBackend* Context::GetBackend() const noexcept
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
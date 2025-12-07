#include "Engine.h"
#include <iostream>
#include "Core/Window/WindowSubsystem.h"
#include "Core/Window/OpenGL/OpenGLBackend.h"
#include "Core/Window/GraphicWindow.h"
#include "OpenGLRenderBackend/Context/Context.h"
#include "RenderEngine/RenderQueue/RenderQueue.h"

/*
TestRenderPass:
    TestPipeline: Draw texture into image -> set resolve and screenshot renderpasses state
ScreenShotRenderPass:
    ScreenShotPipeline: Draw blit textures from state to image -> then copy 
ResolveRenderPass:
    ResolvePipeline: Draw blit textures from state to default framebuffer 

*/

#include "RenderEngine/Tasks/RenderPassTask.h"
#include "RenderEngine/Tasks/GraphicsPipelineTask.h"
#include "Core/TGA/TGA.h"

#include "hrs/exe_path.hpp"

class ResolveRenderPass : public RenderPassTask
{
    static Render::RenderPassInfo get_info()
    {
        static Render::AttachmentDescription desc = {.clear_load = false};
        return Render::RenderPassInfo{.color_attachment_descriptions = {&desc, 1},
                                      .depth_stencil_attachment_description = nullptr};
    }
public:
    class State
    {
        friend class ResolveRenderPass;
    public:
        State() noexcept
            : input_image(nullptr)
        {}

        ~State() = default;

        Render::ImageView* input_image;
    private:
        void Clear()
        {
            input_image = nullptr;
        }
    };

    ResolveRenderPass(RenderQueue* _parent, TaskKey&& key)
        : RenderPassTask(_parent, std::move(key), get_info())
    {}

    virtual ~ResolveRenderPass() override = default;

    EvaluateDesc Begin(const EvaluateDesc& eval_desc) override
    {
        const Render::RenderPassBeginInfo info = {
            .framebuffer =
                GetRoot()->As<RenderEngine>()->GetContext()->GetCurrentDefaultFramebuffer(),
            .clear_color_values = {},
            .clear_depth_stencil_value = Render::ClearDepthStencilValue{}};
        handle->Begin(eval_desc.cmd, info);

        return eval_desc;
    }

    void End(const EvaluateDesc& eval_desc) override
    {
        handle->End(eval_desc.cmd);

        state.Clear();
    }

    State& GetState() noexcept
    {
        return state;
    }
private:
    State state;
};

CHECK_TASK_IS_READY(ResolveRenderPass)

class ResolvePipeline : public GraphicsPipelineTask,
                        public Events::EventListener<WindowResizedEvent>
{
    static Render::GraphicsPipelineInfo get_info()
    {
        auto vert = Engine::GetInstance()->GetResourceManager()->CreateShader("blit.vert");
        auto frag = Engine::GetInstance()->GetResourceManager()->CreateShader("blit.frag");

        static std::array shaders = {vert->GetShader(), frag->GetShader()};

        return Render::GraphicsPipelineInfo{
            .shaders = {shaders.data(), shaders.size()},
            .state_info = Render::GraphicsPipelineStateInfo{
                .vertex_input_state_info = Render::GraphicsPipelineVertexInputStateInfo{},
                .input_assembly_state_info =
                    Render::GraphicsPipelineInputAssemblyStateInfo{
                        .topology = Render::PrimitiveTopology::Triangles,
                        .primitive_restart_enabled = false},
                .blend_state_info = Render::GraphicsPipelineBlendStateInfo{.blend_enabled = false},
                .depth_stencil_state_info =
                    Render::GraphicsPipelineDepthStencilStateInfo{.depth_test_enabled = false,
                                                                  .stencil_test_enabled = false},
                .multisample_state_info =
                    Render::GraphicsPipelineMultisampleStateInfo{.multisample_enabled = false},
                .rasterization_state_info =
                    Render::GraphicsPipelineRasterizationStateInfo{
                        .depth_clamp_enabled = false,
                        .rasterizer_discard_enabled = false,
                        .polygon_mode = Render::PolygonMode::Fill,
                        .cull_mode = Render::CullMode::None,
                        .front_face = Render::FrontFace::Clockwise,
                        .line_width = 1.0f},
                .viewport_state_info =
                    Render::GraphicsPipelineViewportStateInfo{.viewport_enabled = false}}};
    }
public:
    ResolvePipeline(ResolveRenderPass* _parent, TaskKey&& key)
        : GraphicsPipelineTask(_parent, std::move(key), get_info())
    {
        WindowResolution resolution = Engine::GetInstance()->GetWindow()->GetDrawableResolution();
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = resolution.width;
        viewport.height = resolution.height;
        viewport.min_depth = 0.0f;
        viewport.max_depth = 1.0f;

        scissors.extent.width = resolution.width;
        scissors.extent.height = resolution.height;
        scissors.offset.x = 0;
        scissors.offset.y = 0;

        Events::Connect<WindowResizedEvent>(this,
                                            Engine::GetInstance()->GetWindow(),
                                            &ResolvePipeline::Handle);
    }

    virtual ~ResolvePipeline() override = default;

    EvaluateDesc Begin(const EvaluateDesc& eval_desc) override
    {
        constexpr Render::UniformDesc uniform_desc = {.type = Render::UniformType::UInt,
                                                      .extent = Render::UniformExtent::Scalar,
                                                      .count = 1,
                                                      .location = 0};

        std::int32_t msaa_enabled = false;

        Render::ImageView* input_image =
            GetParent()->As<ResolveRenderPass>()->GetState().input_image;
        if(input_image)
        {
            handle->Bind(eval_desc.cmd);
            handle->SetViewport(eval_desc.cmd, 0, {&viewport, 1});
            handle->SetScissor(eval_desc.cmd, 0, {&scissors, 1});

            handle->BindImageView(eval_desc.cmd, input_image, 0);

            handle->SetUniform(
                eval_desc.cmd,
                uniform_desc,
                {reinterpret_cast<const std::byte*>(&msaa_enabled), sizeof(std::int32_t)});
            handle->Draw(eval_desc.cmd, 6, 1, 0);
        }
        return EvaluateDesc{.cmd = eval_desc.cmd, .pipeline = nullptr};
    }

    void End(const EvaluateDesc& eval_desc) override
    {
        //noop
    }
private:
    Events::HandlerAction Handle(const WindowResizedEvent& event) noexcept
    {
        viewport.width = event.drawable_resolution.width;
        viewport.height = event.drawable_resolution.height;

        scissors.extent.width = event.drawable_resolution.width;
        scissors.extent.height = event.drawable_resolution.height;

        return Events::HandlerAction::None;
    }
private:
    Render::Viewport viewport;
    Render::Rect2D scissors;
};

CHECK_TASK_IS_READY(ResolvePipeline)

class ScreenShotTask : public TaskLeaf,
                       public Events::EventListener<WindowResizedEvent>,
                       public Events::EventListener<MouseButtonEvent>
{
public:
    class State
    {
        friend class ScreenShotTask;
    public:
        State() noexcept
            : input_image(nullptr)
        {}

        ~State() = default;

        Render::ImageView* input_image;
    private:
        void Clear()
        {
            input_image = nullptr;
        }
    };

    ScreenShotTask(RenderQueue* _parent, TaskKey&& key)
        : TaskLeaf(_parent, std::move(key)),
          in_progress_frame_index(-1),
          is_requested(false)
    {
        auto res = Engine::GetInstance()->GetWindow()->GetDrawableResolution();
        const Render::AttachmentDescription desc = {.clear_load = false};

        renderpass = GetRoot()->As<RenderEngine>()->GetContext()->CreateRenderPassUnique(
            Render::RenderPassInfo{.color_attachment_descriptions = {&desc, 1},
                                   .depth_stencil_attachment_description = nullptr});

        auto vert = Engine::GetInstance()->GetResourceManager()->CreateShader("blit.vert");
        auto frag = Engine::GetInstance()->GetResourceManager()->CreateShader("blit.frag");

        static std::array shaders = {vert->GetShader(), frag->GetShader()};

        pipeline = GetRoot()->As<RenderEngine>()->GetContext()->CreatePipelineUnique(
            Render::GraphicsPipelineInfo{
                .shaders = {shaders.data(), shaders.size()},
                .state_info = Render::GraphicsPipelineStateInfo{
                    .vertex_input_state_info = Render::GraphicsPipelineVertexInputStateInfo{},
                    .input_assembly_state_info =
                        Render::GraphicsPipelineInputAssemblyStateInfo{
                            .topology = Render::PrimitiveTopology::Triangles,
                            .primitive_restart_enabled = false},
                    .blend_state_info =
                        Render::GraphicsPipelineBlendStateInfo{.blend_enabled = false},
                    .depth_stencil_state_info =
                        Render::GraphicsPipelineDepthStencilStateInfo{.depth_test_enabled = false,
                                                                      .stencil_test_enabled =
                                                                          false},
                    .multisample_state_info =
                        Render::GraphicsPipelineMultisampleStateInfo{.multisample_enabled = false},
                    .rasterization_state_info =
                        Render::GraphicsPipelineRasterizationStateInfo{
                            .depth_clamp_enabled = false,
                            .rasterizer_discard_enabled = false,
                            .polygon_mode = Render::PolygonMode::Fill,
                            .cull_mode = Render::CullMode::None,
                            .front_face = Render::FrontFace::Clockwise,
                            .line_width = 1.0f},
                    .viewport_state_info =
                        Render::GraphicsPipelineViewportStateInfo{.viewport_enabled = false}}});

        recreate_images(Engine::GetInstance()->GetWindow()->GetDrawableResolution());

        Events::Connect<WindowResizedEvent>(this,
                                            Engine::GetInstance()->GetWindow(),
                                            &ScreenShotTask::Handle);

        Events::Connect<MouseButtonEvent>(this,
                                          Engine::GetInstance()->GetWindow(),
                                          &ScreenShotTask::Handle);
    }

    ~ScreenShotTask() = default;

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) override
    {
        if(in_progress_frame_index != -1 &&
           in_progress_frame_index == GetRoot()->As<RenderEngine>()->GetCurrentResourceSetIndex())
        {
            flush();
        }

        if(is_requested && state.input_image != nullptr)
        {
            const Render::RenderPassBeginInfo info = {.framebuffer = framebuffer.get(),
                                                      .clear_color_values = {},
                                                      .clear_depth_stencil_value = {}};
            renderpass->Begin(eval_desc.cmd, info);

            constexpr Render::UniformDesc uniform_desc = {.type = Render::UniformType::UInt,
                                                          .extent = Render::UniformExtent::Scalar,
                                                          .count = 1,
                                                          .location = 0};

            std::int32_t msaa_enabled = false;

            pipeline->Bind(eval_desc.cmd);
            pipeline->SetViewport(eval_desc.cmd, 0, {&viewport, 1});
            pipeline->SetScissor(eval_desc.cmd, 0, {&scissors, 1});
            pipeline->BindImageView(eval_desc.cmd, state.input_image, 0);

            pipeline->SetUniform(
                eval_desc.cmd,
                uniform_desc,
                {reinterpret_cast<const std::byte*>(&msaa_enabled), sizeof(std::int32_t)});
            pipeline->Draw(eval_desc.cmd, 6, 1, 0);

            const Render::BufferImageCopyRegion region = {
                .buffer_offset = 0,
                .buffer_row_length = scissors.extent.width,
                .buffer_image_height = scissors.extent.height,
                .subresource_layers = Render::ImageSubresourceLayers{.mip_level = 0,
                                                                     .base_layer = 0,
                                                                     .layer_count = 1},
                .offset = Render::Offset3D{.x = 0, .y = 0, .z = 0},
                .extent = Render::Extent3D{.width = scissors.extent.width,
                                           .height = scissors.extent.height,
                                           .depth = 1}};

            image->CopyToBuffer(eval_desc.cmd, buffer.get(), {&region, 1});

            in_progress_frame_index = GetRoot()->As<RenderEngine>()->GetCurrentResourceSetIndex();
            is_requested = false;

            state.Clear();

            return EvaluateDesc{.cmd = eval_desc.cmd, .pipeline = pipeline.get()};
        }

        return eval_desc;
    }

    virtual void End(const EvaluateDesc& eval_desc) override
    {
        renderpass->End(eval_desc.cmd);

        state.input_image = nullptr; //clear state
    }

    State& GetState() noexcept
    {
        return state;
    }
private:
    Events::HandlerAction Handle(const WindowResizedEvent& event)
    {
        if(in_progress_frame_index != -1)
            flush();

        recreate_images(event.drawable_resolution);

        return Events::HandlerAction::None;
    }

    Events::HandlerAction Handle(const MouseButtonEvent& event)
    {
        if(event.state == ButtonState::Pressed)
            is_requested = true;

        return Events::HandlerAction::None;
    }

    void flush()
    {
#pragma warning("BARRIER???!!!")
        //#error SWAP R AND B!!! -> TGA have BGRA order!!!
        auto mapped_ptr = buffer->Map(Render::MappedRange{.offset = 0, .size = buffer->GetSize()});
        for(std::size_t i = 0; i < buffer->GetSize(); i += 4)
            std::swap(mapped_ptr[i], mapped_ptr[i + 2]);

        auto now = std::chrono::system_clock::now();

        auto filename = std::format(
            "userdata/screenshots/screenshot_{0:%d}.{0:%m}.{0:%Y}T{0:%H}.{0:%M}.{0:%S}.tga",
            now);

        std::filesystem::create_directories("userdata/screenshots/");

        const TGA::WriteInfo write_info = {
            .filename = filename.c_str(),
            .width = static_cast<std::uint16_t>(scissors.extent.width),
            .height = static_cast<std::uint16_t>(scissors.extent.height),
            .data_type = TGA::DataType::UncompressedRGB,
            .pixel_depth = TGA::PixelDepth::Bits32,
            .top_to_bottom = false,
            .data = mapped_ptr,
            .id = ""};

        auto res = TGA::Write(write_info);
        buffer->Unmap();
        if(res.has_value())
            throw res.value();

        in_progress_frame_index = -1;
    }

    void recreate_images(const WindowResolution& res)
    {
        viewport = Render::Viewport{.x = 0,
                                    .y = 0,
                                    .width = static_cast<float>(res.width),
                                    .height = static_cast<float>(res.height),
                                    .min_depth = 0.0f,
                                    .max_depth = 1.0f};

        scissors =
            Render::Rect2D{.offset = Render::Offset2D{.x = 0, .y = 0},
                           .extent = Render::Extent2D{.width = static_cast<uint32_t>(res.width),
                                                      .height = static_cast<uint32_t>(res.height)}};

        framebuffer.reset();
        image_view.reset();
        image.reset();
        buffer.reset();

        image = GetRoot()->As<RenderEngine>()->GetContext()->CreateImageUnique(Render::ImageInfo{
            .image_type = Render::ImageType::Image2D,
            .format = Render::Format::R8G8B8A8_UNORM,
            .extent = Render::Extent3D{.width = static_cast<uint32_t>(res.width),
                                       .height = static_cast<uint32_t>(res.height),
                                       .depth = 1},
            .mip_levels = 1,
            .array_layers = 1,
            .samples = Render::SampleCount::SampleCount_1});

        image_view = GetRoot()->As<RenderEngine>()->GetContext()->CreateImageViewUnique(
            Render::ImageViewInfo{.image = image.get(),
                                  .view_type = Render::ImageViewType::ImageView2D,
                                  .format = Render::Format::R8G8B8A8_UNORM,
                                  .components =
                                      Render::ComponentMapping{
                                          .r = Render::ComponentSwizzle::SwizzleIdentity,
                                          .g = Render::ComponentSwizzle::SwizzleIdentity,
                                          .b = Render::ComponentSwizzle::SwizzleIdentity,
                                          .a = Render::ComponentSwizzle::SwizzleIdentity,
                                      },
                                  .subresource_range =
                                      Render::ImageSubresourceRange{.min_mip_level = 0,
                                                                    .mip_level_count = 1,
                                                                    .min_layer = 0,
                                                                    .layer_count = 1}});
        std::array attachments = {Render::AttachmentRef{.attachment = image_view.get()}};
        framebuffer = GetRoot()->As<RenderEngine>()->GetContext()->CreateFramebufferUnique(
            Render::FramebufferInfo{.color_attachments = attachments,
                                    .depth_stencil_attachment = nullptr});

        buffer = GetRoot()->As<RenderEngine>()->GetContext()->CreateBufferUnique(Render::BufferInfo{
            .size = static_cast<uint64_t>(
                res.width * res.height *
                (Render::GetFormatBitsPerPixel(Render::Format::R8G8B8A8_UNORM) / 8)),
            .flags = Render::BufferFlagBits::MapRead});
    }
private:
    std::int32_t in_progress_frame_index;
    bool is_requested;

    Render::Viewport viewport;
    Render::Rect2D scissors;

    std::unique_ptr<Render::RenderPass> renderpass;
    std::unique_ptr<Render::Pipeline> pipeline;

    std::unique_ptr<Render::Image> image;
    std::unique_ptr<Render::ImageView> image_view;
    std::unique_ptr<Render::Framebuffer> framebuffer;

    std::unique_ptr<Render::Buffer> buffer;

    State state;
};

CHECK_TASK_IS_READY(ScreenShotTask)

class TestRenderPass : public RenderPassTask, public Events::EventListener<WindowResizedEvent>
{
    constexpr static Render::RenderPassInfo get_info()
    {
        Render::AttachmentDescription desc = {.clear_load = true};
        return Render::RenderPassInfo{.color_attachment_descriptions = {&desc, 1},
                                      .depth_stencil_attachment_description = nullptr};
    }
public:
    class State
    {
        friend class TestRenderPass;
    public:
        State() noexcept
            : output_image(nullptr)
        {}

        ~State() = default;

        Render::ImageView* output_image;
    };

    TestRenderPass(RenderQueue* _parent, TaskKey&& key)
        : RenderPassTask(_parent, std::move(key), get_info())
    {
        recreate_images(Engine::GetInstance()->GetWindow()->GetDrawableResolution(),
                        GetRoot()->As<RenderEngine>());

        Events::Connect<WindowResizedEvent>(this,
                                            Engine::GetInstance()->GetWindow(),
                                            &TestRenderPass::Handle);
    }

    virtual ~TestRenderPass() override
    {}

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) override
    {
        constexpr std::array colors = {
            //Render::ClearColorValue{.value = Render::ClearColorFloatValue{0.0f, 0.0f, 0.0f, 0.0f}},
            //Render::ClearColorValue{.value = Render::ClearColorFloatValue{1.0f, 0.0f, 0.0f, 0.0f}},
            //Render::ClearColorValue{.value = Render::ClearColorFloatValue{0.0f, 1.0f, 0.0f, 0.0f}},
            //Render::ClearColorValue{.value = Render::ClearColorFloatValue{0.0f, 0.0f, 1.0f, 0.0f}},
            //Render::ClearColorValue{.value = Render::ClearColorFloatValue{1.0f, 1.0f, 0.0f, 0.0f}},
            Render::ClearColorValue{.value = Render::ClearColorFloatValue{1.0f, 0.0f, 1.0f, 0.0f}},
            //Render::ClearColorValue{.value = Render::ClearColorFloatValue{0.0f, 1.0f, 1.0f, 0.0f}},
            //Render::ClearColorValue{.value = Render::ClearColorFloatValue{1.0f, 1.0f, 1.0f, 0.0f}}
        };

        static std::size_t i = 0;
        static auto prev = std::chrono::system_clock::now();

        auto now = std::chrono::system_clock::now();
        if(std::chrono::duration<float>(now - prev).count() >= 1)
        {
            i = (i + 1) % colors.size();

            prev = now;
        }

        const Render::RenderPassBeginInfo info = {
            .framebuffer = resources[GetRoot()->As<RenderEngine>()->GetCurrentResourceSetIndex()]
                               .framebuffer.get(),
            .clear_color_values = {&colors[i], 1},
            .clear_depth_stencil_value = Render::ClearDepthStencilValue{}};

        handle->Begin(eval_desc.cmd, info);

        state.output_image =
            resources[GetRoot()->As<RenderEngine>()->GetCurrentResourceSetIndex()].image_view.get();

        return eval_desc;
    }

    virtual void End(const EvaluateDesc& eval_desc) override
    {
        handle->End(eval_desc.cmd);
    }

    State& GetState() noexcept
    {
        return state;
    }
private:
    struct PerFrameResource
    {
        std::unique_ptr<Render::Image> image;
        std::unique_ptr<Render::ImageView> image_view;
        std::unique_ptr<Render::Framebuffer> framebuffer;
    };

    Events::HandlerAction Handle(const WindowResizedEvent& event)
    {
        recreate_images(event.drawable_resolution, GetRoot()->As<RenderEngine>());

        return Events::HandlerAction::None;
    }

    void recreate_images(const WindowResolution& res, RenderEngine* engine)
    {
        resources.clear();

        auto frame_count = engine->GetResourceSetCount();

        resources.reserve(frame_count);

        const Render::ImageInfo image_info = {
            .image_type = Render::ImageType::Image2D,
            .format = Render::Format::R8G8B8A8_SNORM,
            .extent = Render::Extent3D{.width = static_cast<std::uint32_t>(res.width),
                                       .height = static_cast<std::uint32_t>(res.height),
                                       .depth = 1},
            .mip_levels = 1,
            .array_layers = 1,
            .samples = Render::SampleCount::SampleCount_1};

        Render::ImageViewInfo image_view_info = {
            .image = nullptr,
            .view_type = Render::ImageViewType::ImageView2D,
            .format = Render::Format::R8G8B8A8_SNORM,
            .components = Render::ComponentMapping{.r = Render::ComponentSwizzle::SwizzleIdentity,
                                                   .g = Render::ComponentSwizzle::SwizzleIdentity,
                                                   .b = Render::ComponentSwizzle::SwizzleIdentity,
                                                   .a = Render::ComponentSwizzle::SwizzleIdentity},
            .subresource_range = Render::ImageSubresourceRange{.min_mip_level = 0,
                                                               .mip_level_count = 1,
                                                               .min_layer = 0,
                                                               .layer_count = 1}};

        Render::FramebufferInfo fb_info = {.color_attachments = {},
                                           .depth_stencil_attachment = nullptr};
        for(std::size_t i = 0; i < frame_count; i++)
        {
            resources.push_back({});
            resources.back().image = engine->GetContext()->CreateImageUnique(image_info);

            image_view_info.image = resources.back().image.get();
            resources.back().image_view =
                engine->GetContext()->CreateImageViewUnique(image_view_info);

            Render::AttachmentRef att_ref{.attachment = resources.back().image_view.get()};
            fb_info.color_attachments = {&att_ref, 1};
            resources.back().framebuffer = engine->GetContext()->CreateFramebufferUnique(fb_info);
        }
    }
private:
    std::vector<PerFrameResource> resources;

    State state;
};

CHECK_TASK_IS_READY(TestRenderPass)

class TestPipeline : public GraphicsPipelineTask, public Events::EventListener<WindowResizedEvent>
{
    static Render::GraphicsPipelineInfo get_info()
    {
        auto vert = Engine::GetInstance()->GetResourceManager()->CreateShader("test.vert");
        auto frag = Engine::GetInstance()->GetResourceManager()->CreateShader("test.frag");

        static std::array shaders = {vert->GetShader(), frag->GetShader()};

        return Render::GraphicsPipelineInfo{
            .shaders = {shaders.data(), shaders.size()},
            .state_info = Render::GraphicsPipelineStateInfo{
                .vertex_input_state_info = Render::GraphicsPipelineVertexInputStateInfo{},
                .input_assembly_state_info =
                    Render::GraphicsPipelineInputAssemblyStateInfo{
                        .topology = Render::PrimitiveTopology::Triangles,
                        .primitive_restart_enabled = false},
                .blend_state_info = Render::GraphicsPipelineBlendStateInfo{.blend_enabled = false},
                .depth_stencil_state_info =
                    Render::GraphicsPipelineDepthStencilStateInfo{.depth_test_enabled = false,
                                                                  .stencil_test_enabled = false},
                .multisample_state_info =
                    Render::GraphicsPipelineMultisampleStateInfo{.multisample_enabled = false},
                .rasterization_state_info =
                    Render::GraphicsPipelineRasterizationStateInfo{
                        .depth_clamp_enabled = false,
                        .rasterizer_discard_enabled = false,
                        .polygon_mode = Render::PolygonMode::Fill,
                        .cull_mode = Render::CullMode::None,
                        .front_face = Render::FrontFace::Clockwise,
                        .line_width = 1.0f},
                .viewport_state_info =
                    Render::GraphicsPipelineViewportStateInfo{.viewport_enabled = false}}};
    }
public:
    TestPipeline(TestRenderPass* _parent, TaskKey&& key)
        : GraphicsPipelineTask(_parent, std::move(key), get_info())
    {
        image = Engine::GetInstance()->GetResourceManager()->CreateImage("test.dds",
                                                                         TransferMode::Storage);

        sampler = GetRoot()->As<RenderEngine>()->GetContext()->CreateSamplerUnique(
            Render::SamplerInfo{.mag_filter = Render::Filter::Nearest,
                                .min_filter = Render::Filter::Linear,
                                .mipmap_mode = Render::Filter::Linear,
                                .address_mode_u = Render::AddressMode::ClampToEdge,
                                .address_mode_v = Render::AddressMode::ClampToEdge,
                                .address_mode_w = Render::AddressMode::ClampToEdge,
                                .mip_lod_bias = 1.0f,
                                .anisotropy_enable = false,
                                .max_anisotropy = 1.0f,
                                .compare_enable = false,
                                .min_lod = -1000.0f,
                                .max_lod = 1000.0f,
                                .border_color = Render::BorderColor{}});

        WindowResolution resolution = Engine::GetInstance()->GetWindow()->GetDrawableResolution();
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = resolution.width;
        viewport.height = resolution.height;
        viewport.min_depth = 0.0f;
        viewport.max_depth = 1.0f;

        scissors.extent.width = resolution.width;
        scissors.extent.height = resolution.height;
        scissors.offset.x = 0;
        scissors.offset.y = 0;

        Events::Connect<WindowResizedEvent>(this,
                                            Engine::GetInstance()->GetWindow(),
                                            &TestPipeline::Handle);
    }

    virtual ~TestPipeline() override = default;

    EvaluateDesc Begin(const EvaluateDesc& eval_desc) override
    {
        //if(image->GetState() == ResourceManager::ResourceState::NotReady)//do not handle this right now!?
        handle->Bind(eval_desc.cmd);
        handle->SetViewport(eval_desc.cmd, 0, {&viewport, 1});
        handle->SetScissor(eval_desc.cmd, 0, {&scissors, 1});
        handle->BindImageView(eval_desc.cmd, image->GetImageView(), 0);
        handle->BindSampler(eval_desc.cmd, sampler.get(), 0);

        handle->Draw(eval_desc.cmd, 6, 1, 0);

        return EvaluateDesc{.cmd = eval_desc.cmd, .pipeline = handle.get()};
    }

    void End(const EvaluateDesc& eval_desc) override
    {
        //noop
    }
private:
    Events::HandlerAction Handle(const WindowResizedEvent& event) noexcept
    {
        viewport.width = event.drawable_resolution.width;
        viewport.height = event.drawable_resolution.height;

        scissors.extent.width = event.drawable_resolution.width;
        scissors.extent.height = event.drawable_resolution.height;

        return Events::HandlerAction::None;
    }
private:
    Render::Viewport viewport;
    Render::Rect2D scissors;
    hrs::rc_ptr<ResourceManager::ImageResource> image;
    std::unique_ptr<Render::Sampler> sampler;
};

CHECK_TASK_IS_READY(TestPipeline)

#include "RenderEngine/TaskTree/TaskState.hpp"

FUNCTIONAL_STATE_TRANSFER(TestToResolve, TestRenderPass, ResolveRenderPass, {
    consumer->GetState().input_image = producer->GetState().output_image;
})

FUNCTIONAL_STATE_TRANSFER(TestToScreenshot, TestRenderPass, ScreenShotTask, {
    consumer->GetState().input_image = producer->GetState().output_image;
})

#define SHOW_INIT_ERROR(STR, ...) \
    if(!WindowSubsystem::ShowMessageBox(nullptr, \
                                        MessageBoxType::Error, \
                                        "Init failure", \
                                        STR __VA_OPT__(, __VA_ARGS__))) \
        std::cerr << STR __VA_OPT__(, __VA_ARGS__) << std::endl; // as fallback -> make logger!!!

//transfer_queue_sem -> render_queue_sem
//render_engine -> swapchain

class SwapchainToRenderQueueStateTransfer : public StateTransfer<RenderEngine, RenderQueue>
{
public:
    SwapchainToRenderQueueStateTransfer(RenderEngine* _producer,
                                        RenderQueue* _consumer,
                                        TaskBase* _parent,
                                        TaskKey&& key)
        : StateTransfer<RenderEngine, RenderQueue>(_producer, _consumer, _parent, std::move(key))
    {
        auto engine = GetRoot()->As<RenderEngine>();

        semaphores.reserve(engine->GetResourceSetCount());
        fences.reserve(engine->GetResourceSetCount());
        for(std::size_t i = 0; i < engine->GetResourceSetCount(); i++)
        {
            semaphores.push_back(engine->GetContext()->CreateSemaphoreUnique());
            fences.push_back(engine->GetContext()->CreateFenceUnique());
        }
    }

    ~SwapchainToRenderQueueStateTransfer()
    {
        auto engine = GetRoot()->As<RenderEngine>();
        engine->GetContext()->WaitIdle();
    }

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) override
    {
        auto sem = semaphores[GetRoot()->As<RenderEngine>()->GetCurrentResourceSetIndex()].get();
        auto fence = fences[GetRoot()->As<RenderEngine>()->GetCurrentResourceSetIndex()].get();

        producer->GetContext()->AcquireNextSwapchainImage(sem);
        consumer->GetState().wait_semaphores.push_back(sem);
        consumer->GetState().signal_fence = fence;

        return eval_desc;
    }
private:
    std::vector<std::unique_ptr<Render::Semaphore>> semaphores;
    std::vector<std::unique_ptr<Render::Fence>> fences;
};

class TranfserQueueFenceStateTransfer : public StateTransfer<RenderEngine, TransferQueue>
{
public:
    TranfserQueueFenceStateTransfer(RenderEngine* _producer,
                                    TransferQueue* _consumer,
                                    TaskBase* _parent,
                                    TaskKey&& key)
        : StateTransfer<RenderEngine, TransferQueue>(_producer, _consumer, _parent, std::move(key))
    {
        auto engine = GetRoot()->As<RenderEngine>();

        fences.reserve(engine->GetResourceSetCount());
        for(std::size_t i = 0; i < engine->GetResourceSetCount(); i++)
        {
            fences.push_back(engine->GetContext()->CreateFenceUnique());
        }
    }

    ~TranfserQueueFenceStateTransfer()
    {
        auto engine = GetRoot()->As<RenderEngine>();
        engine->GetContext()->WaitIdle();
    }

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) override
    {
        auto fence = fences[GetRoot()->As<RenderEngine>()->GetCurrentResourceSetIndex()].get();

        consumer->GetState().signal_fence = fence;

        return eval_desc;
    }
private:
    std::vector<std::unique_ptr<Render::Fence>> fences;
};

class TransferQueueToRenderQueueStateTransfer : public StateTransfer<TransferQueue, RenderQueue>
{
public:
    TransferQueueToRenderQueueStateTransfer(TransferQueue* _producer,
                                            RenderQueue* _consumer,
                                            TaskBase* _parent,
                                            TaskKey&& key)
        : StateTransfer<TransferQueue, RenderQueue>(_producer, _consumer, _parent, std::move(key))
    {
        auto engine = GetRoot()->As<RenderEngine>();

        semaphores.reserve(engine->GetResourceSetCount());
        for(std::size_t i = 0; i < engine->GetResourceSetCount(); i++)
        {
            semaphores.push_back(engine->GetContext()->CreateSemaphoreUnique());
        }
    }

    ~TransferQueueToRenderQueueStateTransfer()
    {
        auto engine = GetRoot()->As<RenderEngine>();
        engine->GetContext()->WaitIdle();
    }

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) override
    {
        auto sem = semaphores[GetRoot()->As<RenderEngine>()->GetCurrentResourceSetIndex()].get();

        producer->GetState().signal_semaphores.push_back(sem);
        consumer->GetState().wait_semaphores.push_back(sem);

        return eval_desc;
    }
private:
    std::vector<std::unique_ptr<Render::Semaphore>> semaphores;
};

class RenderQueueToSwapchainStateTransfer : public StateTransfer<RenderQueue, RenderEngine>
{
public:
    RenderQueueToSwapchainStateTransfer(RenderQueue* _producer,
                                        RenderEngine* _consumer,
                                        TaskBase* _parent,
                                        TaskKey&& key)
        : StateTransfer<RenderQueue, RenderEngine>(_producer, _consumer, _parent, std::move(key))
    {
        auto engine = GetRoot()->As<RenderEngine>();

        semaphores.reserve(engine->GetResourceSetCount());
        for(std::size_t i = 0; i < engine->GetResourceSetCount(); i++)
        {
            semaphores.push_back(engine->GetContext()->CreateSemaphoreUnique());
        }
    }

    ~RenderQueueToSwapchainStateTransfer()
    {
        auto engine = GetRoot()->As<RenderEngine>();
        engine->GetContext()->WaitIdle();
    }

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) override
    {
        auto sem = semaphores[GetRoot()->As<RenderEngine>()->GetCurrentResourceSetIndex()].get();

        producer->GetState().signal_semaphores.push_back(sem);

        const Render::PresentInfo present_info = {.wait_semaphores = {&sem, 1}};
        consumer->GetContext()->ReleaseSwapchainImage(present_info);

        return eval_desc;
    }
private:
    std::vector<std::unique_ptr<Render::Semaphore>> semaphores;
};

Engine::Engine()
{
    instance = this;

    WindowSubsystem* win_sys = WindowSubsystem::Init();
    const GraphicWindowInfo window_info =
        GraphicWindowInfo{.resolution = WindowResolution{.width = 800, .height = 600},
                          .title = "game"};

    const OpenGLBackendInfo render_info = OpenGLBackendInfo{
        RenderBackendInfo{.type = RenderBackendType::OpenGL},
        OpenGLBackendDefaultFramebufferInfo{.format = Render::Format::R8G8B8A8_UNORM},
        OpenGLVersion::OpenGL_4_6_Core,
        OpenGLBackendFlagBits::DebugContext};

    window = win_sys->CreateGraphicWindow(window_info, render_info);

    std::unique_ptr<OpenGL::Context> _context(
        new OpenGL::Context(static_cast<OpenGLBackend*>(window->GetRenderBackend())));

    std::array shader_resource_descs = {
        ResourceExtensionDesc<ShaderResourceDesc>{
            .ext = ".vert",
            .desc = ShaderResourceDesc{.stage = Render::ShaderStage::Vertex}},
        ResourceExtensionDesc<ShaderResourceDesc>{
            .ext = ".frag",
            .desc = ShaderResourceDesc{.stage = Render::ShaderStage::Fragment}},
    };

    const RenderEngineInfo render_engine_info = RenderEngineInfo{.resource_set_count = 3};
    render_engine = std::unique_ptr<RenderEngine>(
        new TaskRootWrapper<RenderEngine>(render_engine_info, std::move(_context), window));

    const Render::DebugMessengerInfo debug_messenger_info = {
        .severities = Render::DebugMessengerSeverityFlagBits::Info |
                      Render::DebugMessengerSeverityFlagBits::Error |
                      Render::DebugMessengerSeverityFlagBits::Verbose |
                      Render::DebugMessengerSeverityFlagBits::Warning,
        .types = Render::DebugMessengerTypeFlagBits::General |
                 Render::DebugMessengerTypeFlagBits::Performance |
                 Render::DebugMessengerTypeFlagBits::Validation,
        .callback = [](Render::DebugMessengerSeverityFlagBits severity,
                       Render::DebugMessengerTypeFlags types,
                       std::int64_t id,
                       std::string_view message)
        {
            std::string_view severity_name;
            switch(severity)
            {
                case Render::DebugMessengerSeverityFlagBits::Info:
                    severity_name = "Info";
                    break;
                case Render::DebugMessengerSeverityFlagBits::Error:
                    severity_name = "Error";
                    break;
                case Render::DebugMessengerSeverityFlagBits::Verbose:
                    severity_name = "Verbose";
                    break;
                case Render::DebugMessengerSeverityFlagBits::Warning:
                    severity_name = "Warning";
                    break;
            }

            std::string_view type_name;
            switch(types) //just select cooler one :)
            {
                case Render::DebugMessengerTypeFlagBits::Validation:
                    type_name = "Validation";
                    break;
                case Render::DebugMessengerTypeFlagBits::Performance:
                    type_name = "Performance";
                    break;
                case Render::DebugMessengerTypeFlagBits::General:
                    type_name = "General";
                    break;
            }

            std::cerr << std::format("[{}][{}]({}) -> {}\n", severity_name, type_name, id, message);
        }};
    render_engine->GetContext()->SetDebugMessenger(debug_messenger_info);

    //swapchain -> render_queue
    //fence -> transfer queue
    //transfer queue -> render queue
    //transfer_queue
    //render queue
    //render queue -> swapchain

    auto transfer_queue = new TaskBranchWrapper<TransferQueue>(
        render_engine.get(),
        TaskKey{.priority = 2, .name = std::string_view("TransferQueue")});

    auto transfer_queue_fence_state_transfer = new TranfserQueueFenceStateTransfer(
        render_engine.get(),
        transfer_queue,
        render_engine.get(),
        TaskKey{.priority = transfer_queue->GetPrevPriority(),
                .name = std::string_view("TranfserQueueFenceStateTransfer")});

    auto render_queue = new TaskBranchWrapper<RenderQueue>(
        render_engine.get(),
        TaskKey{.priority = 3, .name = std::string_view("RenderQueue")});

    auto swapchain_to_render_queue_state_transfer = new SwapchainToRenderQueueStateTransfer(
        render_engine.get(),
        render_queue,
        render_engine.get(),
        TaskKey{.priority = 0, .name = std::string_view("SwapchainToRenderQueueStateTransfer")});

    auto transfer_queue_to_render_queue_state_transfer =
        new TransferQueueToRenderQueueStateTransfer(
            transfer_queue,
            render_queue,
            render_engine.get(),
            TaskKey{.priority = transfer_queue->GetPrevPriority(),
                    .name = std::string_view("TransferQueueToRenderQueueStateTransfer")});

    auto render_queue_to_swapchain_state_transfer = new RenderQueueToSwapchainStateTransfer(
        render_queue,
        render_engine.get(),
        render_engine.get(),
        TaskKey{.priority = render_queue->GetNextPriority(),
                .name = std::string_view("RenderQueueToSwapchainStateTransfer")});

    auto executable_path = (hrs::exe_path() / ROOT_SUBDIR).lexically_normal();
    const ResourceManagerInfo resource_manager_info = ResourceManagerInfo{
        .shaders_path_prefix = executable_path / "game/shaders/ogl/compiled",
        .images_path_prefix = executable_path / "game/images",
        .shader_resource_descs = shader_resource_descs,
        .transfer_channel_info =
            TransferChannelInfo{.pool_block_size = 8192, .pool_blocks_reserve = 16}};
    resource_manager = std::unique_ptr<ResourceManager>(new ResourceManager(resource_manager_info));
}

Engine::~Engine()
{}

int Engine::Enter(int argc, char** argv)
{
    int res = 0;
    try
    {
        Engine engine;
        engine.loop();
    }
    catch(const std::exception& ex)
    {
        SHOW_INIT_ERROR(ex.what())
        res = 1;
    }
    catch(...)
    {
        SHOW_INIT_ERROR("Something gone wrong! Aborting...")
        res = 1;
    }

    WindowSubsystem::Close();

    return res;
}

Engine* Engine::GetInstance() noexcept
{
    return instance;
}

GraphicWindow* Engine::GetWindow() const noexcept
{
    return window;
}

ResourceManager* Engine::GetResourceManager() const noexcept
{
    return resource_manager.get();
}

RenderEngine* Engine::GetRenderEngine() const noexcept
{
    return render_engine.get();
}

std::uint64_t Engine::GetFrameIndex() const noexcept
{
    return frame_index;
}

#include <fstream>

void Engine::loop()
{
    auto wm_name = window->GetWindowManagerName();
    std::cerr << std::format("Window manager: {}\n", wm_name);
    auto props = render_engine->GetContext()->GetProperties();
    std::cerr << std::format("Vendor = {}\n", props.vendor_name);
    std::cerr << std::format("Device = {}\n", props.device_name);
    if(props.extensions.empty())
        std::cerr << "No extensions\n";
    else
        std::cerr << "Extensions:\n";

    for(std::size_t i = 0; i < props.extensions.size(); i++)
        std::cerr << std::format("#{:3}: {}\n", i, props.extensions[i]);

    update_factor = REFERENCE_UPDATE_FACTOR;

    constexpr float max_fps = 300.0f;
    auto max_fps_duration = Timer::DurationFromFPS(max_fps);

    bool is_run = true;
    Events::Connect<WindowCloseEvent>(window,
                                      [&is_run](const WindowCloseEvent&)
                                      {
                                          is_run = false;

                                          return Events::HandlerAction::Erase;
                                      });

    auto [obj, index] = render_engine->Query(std::filesystem::path("RenderQueue"));
    if(!obj)
        throw std::runtime_error("Failed to find RenderQueue");

    auto resolve_rpass = new TaskBranchWrapper<ResolveRenderPass>(
        obj->As<RenderQueue>(),
        TaskKey{.priority = 1000, .name = std::string_view("ResolveRenderPass")});

    auto resolve_pipeline = new TaskBranchWrapper<ResolvePipeline>(
        resolve_rpass,
        TaskKey{.priority = 1000, .name = std::string_view("ResolvePipeline")});

    auto screenshot_task =
        new ScreenShotTask(obj->As<RenderQueue>(),
                           TaskKey{.priority = 1000, .name = std::string_view("Screenshot")});

    auto test_rpass = new TaskBranchWrapper<TestRenderPass>(
        obj->As<RenderQueue>(),
        TaskKey{.priority = 0, .name = std::string_view("TestRenderPass")});

    auto test_pipeline = new TaskBranchWrapper<TestPipeline>(
        test_rpass,
        TaskKey{.priority = 0, .name = std::string_view("TestPipeline")});

    auto test_to_resolve =
        new TestToResolve(test_rpass,
                          resolve_rpass,
                          obj->As<RenderQueue>(),
                          TaskKey{.priority = 10, .name = std::string_view("TestToResolve")});

    auto test_to_screenshot =
        new TestToScreenshot(test_rpass,
                             screenshot_task,
                             obj->As<RenderQueue>(),
                             TaskKey{.priority = 10, .name = std::string_view("TestToScreenshot")});

    //#error \
//    "[Error][Validation](1000) -> glBindTextureUnit has generated an error (GL_INVALID_OPERATION)"

    WindowSubsystem* win_sys = WindowSubsystem::GetSubsystem();

    static_cast<OpenGLBackend*>(window->GetRenderBackend())
        ->SetPresentMode(WindowPresentMode::Immediate);

    EngineNextLoopIteratrionEvent next_iter_event = {};

    render_engine->DEBUG_DrawGraph(std::cerr);
    {
        std::ofstream ofs("graph.dot");
        render_engine->DEBUG_DrawGraph(ofs);
    }

    while(is_run)
    {
        frame_timer.Begin();

        Events::Emit(this, next_iter_event);

        resource_manager->GetTransferStorage().Flush();

        render_engine->AcquireNextResourceSet();

        win_sys->PollEvents();

        //render_engine->GetContext()->AcquireNextSwapchainImage(
        //    render_engine->GetRenderQueue()->GetCurrentSwapchainWaitSemaphore());

        EvaluateDesc desc = {.cmd = nullptr, .pipeline = nullptr};
        render_engine->Evaluate(desc);

        //Render::Semaphore* present_wait_sem =
        //    render_engine->GetRenderQueue()->GetCurrentSignalSemaphore();
        //const Render::PresentInfo present_info = {.wait_semaphores = {&present_wait_sem, 1}};
        //render_engine->GetContext()->ReleaseSwapchainImage(present_info);

        frame_timer.End();

        frame_timer.CorrectOnExceeded(max_fps_duration);

        update_factor = frame_timer.AdjustUpdateFactor(REFERENCE_UPDATE_FACTOR);

        auto title =
            std::format("FPS = {:4.3f}, frametime = {:2.3f}ms, update_factor = {:2.3f}",
                        frame_timer.GetFPS(),
                        std::chrono::duration<float, std::milli>(frame_timer.GetDuration()).count(),
                        update_factor);
        window->SetTitle(title.c_str());

        frame_index++;
    }
}
#include "Window/WindowSubsystem.h"
#include "Window/GraphicWindow.h"
#include "Render/Context/Context.h"
#include "Render/Objects/Shader/Shader.h"
#include "Render/Objects/Pipeline/Pipeline.h"
#include "Render/Objects/Buffer/Buffer.h"
#include "Render/Objects/Image/Image.h"
#include "Render/Objects/ImageView/ImageView.h"
#include "Render/Objects/RenderPass/RenderPass.h"
#include "Render/Objects/Sampler/Sampler.h"
#include "Render/Objects/Fence/Fence.h"
#include "Render/Objects/Semaphore/Semaphore.h"
#include "Render/RenderTree/RenderTree.h"

//#include <glad/gl.h>
//#include <SDL2/SDL.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <format>
#include <filesystem>
#include "hrs/math/matrix.hpp"
#include "hrs/math/glsl.hpp"
#include "hrs/math/quaternion.hpp"

int width = 1600;
int height = 1000;

float fov = hrs::math::deg_to_rad(65.0f);
float far = 100.0f;
float near = 0.01f;

struct UniformData
{
    hrs::math::glsl::std140::mat4x4 projection =
        hrs::math::glsl::perspective(fov, near, far, static_cast<float>(width) / height, -1.0f);
    hrs::math::glsl::std140::mat4x4 view =
        hrs::math::glsl::translate(hrs::math::glsl::vec3{0, 0, 0});
};

hrs::math::glsl::mat4x4 view_rotate = hrs::math::glsl::mat4x4::identity();
hrs::math::glsl::mat4x4 view_translate = hrs::math::glsl::mat4x4::identity();
hrs::math::glsl::mat4x4 view = hrs::math::glsl::mat4x4::identity();
bool update_view = true;

void GLAPIENTRY debug_callback(GLenum source,
                               GLenum type,
                               unsigned int id,
                               GLenum severity,
                               GLsizei length,
                               const char* message,
                               const void* userParam)
{
    std::cerr << message << std::endl;
}

#include <fstream>

std::vector<char> ReadFile(const std::filesystem::path& path)
{
    std::ifstream ifs;
    ifs.open(path, std::ios::binary | std::ios::in);
    if(!ifs.is_open())
        throw std::runtime_error(std::format("Failed to open file: {}", path.string()));

    ifs.seekg(0, std::ios::end);
    auto size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> raw(size);
    ifs.read(raw.data(), size);

    return raw;
}

#include "Wavefront/WFObject.h"

////////////////////////

#include "DDS/DDS.h"

////////////////////////

/*using __contract_violation_handler_t = void (*)(const char* condition);

void __default_contract_violation_handler(const char* condition)
{
    std::cerr << condition << std::endl;
    std::terminate();
}

__contract_violation_handler_t __contract_violation_handler = __default_contract_violation_handler;

__contract_violation_handler_t
__set_contract_violation_handler(__contract_violation_handler_t handler) noexcept
{
    return std::exchange(__contract_violation_handler, handler);
}

#define CONTRACT_VIOLATION_POLICY_NOOP

#ifndef CONTRACT_VIOLATION_POLICY_NOOP
#    define CONTRACT(CONDITION, ...) \
        if(!(CONDITION __VA_OPT__(, __VA_ARGS__))) \
            __contract_violation_handler(#CONDITION __VA_OPT__("," #__VA_ARGS__)); \
\
        [[assume((CONDITION __VA_OPT__(, __VA_ARGS__)))]];
#else
#    define CONTRACT(CONDITION, ...)
#endif

void custom_contract_violation_handler(const char* condition)
{
    throw std::runtime_error(std::format("Contract violation: {}", condition));
}

#include "hrs/forward_pool.hpp"

struct A
{
    int i;
};*/

#pragma message("MOVE INIT DATA INTO TRANSFER PASS/CHANNEL!!!")
#pragma message("ADD SPIR-V SHADERS!!!")

std::filesystem::path GAMEDATA_PREFIX = "game";

int main(int argc, char** argv)
{
    //__set_contract_violation_handler(custom_contract_violation_handler);
    //CONTRACT(argc < static_cast<int>(std::same_as<bool, int>));

    try
    {
        const GraphicWindowInfo graphic_window_info = {
            .default_framebuffer_info = DefaultFramebufferInfo{.red_channel_bits = 8,
                                                               .green_channel_bits = 8,
                                                               .blue_channel_bits = 8,
                                                               .alpha_channel_bits = 0,
                                                               .depth_channel_bits = 0,
                                                               .stencil_channel_bits = 0},
            .opengl_version = OpenGLVersion::OpenGL_4_6_Core,
            .resource_set_count = 3,
            .width = static_cast<uint32_t>(width),
            .height = static_cast<uint32_t>(height),
            .title = "game"};

        WindowSubsystem* subsystem = WindowSubsystem::Init(graphic_window_info);
        GraphicWindow* window = subsystem->GetGraphicWindow();
        Context* ctx = window->GetContext();
        std::unique_ptr<RenderTree> render_tree(new RenderTree(ctx));

#pragma message("HERE!")
        ctx->GetLoader().Enable(GL_DEBUG_OUTPUT);
        ctx->GetLoader().Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        ctx->GetLoader().DebugMessageCallback(debug_callback, nullptr);
        ctx->GetLoader()
            .DebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        ctx->GetLoader().Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        ///////////////////////
        struct CubeMap
        {
            Image image;
            ImageView image_view;
        };

        std::unordered_map<std::string, CubeMap> cubemaps;

        std::filesystem::path CUBEMAPS_PREFIX =
            R"(D:/steam/steamapps/common/Stalker Call of Pripyat/UEgamedata/textures/sky)";
        constexpr static auto filenames = {"sky_2_cube.dds",
                                           "sky_3_cube.dds",
                                           "sky_4_cube.dds",
                                           "sky_5_cube.dds",
                                           "sky_6_cube.dds",
                                           "sky_7_cube.dds",
                                           "sky_8_cube.dds",
                                           "sky_9_cube.dds",
                                           "sky_13_cube.dds",
                                           "sky_19_cube.dds",
                                           "sky_18_cube.dds",
                                           "sky_17_cube.dds",
                                           "sky_20_cube.dds",
                                           "sky_21_cube.dds",
                                           "sky_13_cube_night.dds"};
        for(const auto& filename: filenames)
        {
            if(cubemaps.find(filename) != cubemaps.end())
                continue;

            auto raw = ReadFile(CUBEMAPS_PREFIX / filename);

            DDS::DDSResult result =
                DDS::Parse({reinterpret_cast<const std::uint8_t*>(raw.data()), raw.size()});

            DDS::ResolveResult resolve_result = DDS::resolve(result);

            const auto& loader = ctx->GetLoader();

            ImageInitInfo init_info;
            init_info.generate_mip_maps = false;
            init_info.regions = resolve_result.regions;
            if(!IsFormatCompressed(resolve_result.image_info.format))
            {
                init_info.data_format = InitDataFormat::RGBA;
                init_info.data_type = InitDataType::U8;
            }

            Image cubemap(ctx, resolve_result.image_info, init_info);

            const ImageViewInfo view_info{
                .image = &cubemap,
                .view_type = (resolve_result.is_cubemap ? ImageViewType::ImageViewCubeMap :
                                                          ImageViewType::ImageView2D),
                .format = resolve_result.image_info.format,
                .components = ComponentMapping{.r = ComponentSwizzle::SwizzleIdentity,
                                               .g = ComponentSwizzle::SwizzleIdentity,
                                               .b = ComponentSwizzle::SwizzleIdentity,
                                               .a = ComponentSwizzle::SwizzleIdentity},
                .subresource_range = ImageSubresourceRange{
                    .min_mip_level = 0,
                    .mip_level_count = static_cast<GLuint>(resolve_result.image_info.mip_levels),
                    .min_layer = 0,
                    .layer_count = static_cast<GLuint>(resolve_result.image_info.array_layers)}};

            ImageView view_cubemap(ctx, view_info);

            cubemaps.insert(
                {filename,
                 CubeMap{.image = std::move(cubemap), .image_view = std::move(view_cubemap)}});
        }

        constexpr static auto cubemap_order = {"sky_2_cube.dds",
                                               "sky_3_cube.dds",
                                               "sky_4_cube.dds",
                                               "sky_5_cube.dds",
                                               "sky_6_cube.dds",
                                               "sky_7_cube.dds",
                                               "sky_8_cube.dds",
                                               "sky_9_cube.dds",
                                               "sky_13_cube.dds",
                                               "sky_19_cube.dds",
                                               "sky_18_cube.dds",
                                               "sky_17_cube.dds",
                                               "sky_20_cube.dds",
                                               "sky_21_cube.dds",
                                               "sky_13_cube_night.dds"};
        std::vector<CubeMap*> cubemap_order_handles;
        cubemap_order_handles.reserve(cubemap_order.size());
        for(const auto& cubemap_name: cubemap_order)
        {
            auto it = cubemaps.find(cubemap_name);
            if(it == cubemaps.end())
                throw std::runtime_error(std::format("No cubemap: {}", cubemap_name));

            cubemap_order_handles.push_back(&it->second);
        }

        const SamplerInfo cubemap_sampler_info = {.mag_filter = Filter::Linear,
                                                  .min_filter = Filter::Linear,
                                                  .mipmap_mode = Filter::Linear,
                                                  .address_mode_u = AddressMode::ClampToBorder,
                                                  .address_mode_v = AddressMode::ClampToBorder,
                                                  .address_mode_w = AddressMode::ClampToBorder,
                                                  .mip_lod_bias = 0.0f,
                                                  .anisotropy_enable = false,
                                                  .max_anisotropy = 1.0f,
                                                  .compare_enable = false,
                                                  .compare_op = CompareOp::Always,
                                                  .min_lod = 0.0f,
                                                  .max_lod = 1000.0f};

        Sampler cubemap_sampler(ctx, cubemap_sampler_info);

        ///////////////////////

        WFObject obj("game/st.obj");
        auto obj_meshses = obj.GetMeshes();

        if(obj.GetFaceType() != WFObjectFaceType::Vertex_TextureCoordinate_Normal)
            throw std::runtime_error("Bad face type");

        Pipeline pipeline;
        {
            auto vertex_shader_data = ReadFile(GAMEDATA_PREFIX / "geom_vtn.vert");
            auto fragment_shader_data = ReadFile(GAMEDATA_PREFIX / "geom_vtn.frag");

            const ShaderInfo vertex_shader_info = {
                .stage = ShaderStage::Vertex,
                .code = vertex_shader_data.data(),
                .code_size = static_cast<GLint>(vertex_shader_data.size())};
            Shader vertex_shader(ctx, vertex_shader_info);

            const ShaderInfo fragment_shader_info = {
                .stage = ShaderStage::Fragment,
                .code = fragment_shader_data.data(),
                .code_size = static_cast<GLint>(fragment_shader_data.size())};
            Shader fragment_shader(ctx, fragment_shader_info);

            std::array shaders{&vertex_shader, &fragment_shader};

            VertexInputBindingDescription binding = {.binding = 0,
                                                     .stride = sizeof(hrs::math::glsl::vec3) +
                                                               sizeof(hrs::math::glsl::vec2) +
                                                               sizeof(hrs::math::glsl::vec3),
                                                     .input_rate = InputRate::VertexRate};

            std::array attributes = {
                VertexInputAttributeDescription{.location = 0,
                                                .binding = 0,
                                                .size = VertexInputAttributeSize::Vec3,
                                                .type = VertexInputAttributeType::Float,
                                                .offset = 0},
                VertexInputAttributeDescription{.location = 1,
                                                .binding = 0,
                                                .size = VertexInputAttributeSize::Vec2,
                                                .type = VertexInputAttributeType::Float,
                                                .offset = sizeof(hrs::math::glsl::vec3)},
                VertexInputAttributeDescription{.location = 2,
                                                .binding = 0,
                                                .size = VertexInputAttributeSize::Vec3,
                                                .type = VertexInputAttributeType::Float,
                                                .offset = sizeof(hrs::math::glsl::vec3) +
                                                          sizeof(hrs::math::glsl::vec2)}};

            const GraphicsPipelineInfo pipeline_info = {
                .shaders = shaders,
                .state_info = GraphicsPipelineStateInfo{
                    .vertex_input_state_info =
                        GraphicsPipelineVertexInputStateInfo{.vertex_input_bindings = {&binding, 1},
                                                             .vertex_input_attributes = attributes},
                    .input_assembly_state_info =
                        GraphicsPipelineInputAssemblyStateInfo{.topology =
                                                                   PrimitiveTopology::Triangles,
                                                               .primitive_restart_enabled = false},
                    .blend_state_info = GraphicsPipelineBlendStateInfo{.blend_enabled = false},
                    .depth_stencil_state_info =
                        GraphicsPipelineDepthStencilStateInfo{.depth_test_enabled = true,
                                                              .depth_compare_op = CompareOp::Less,
                                                              .write_enabled = true,
                                                              .min_depth_bound = 0.0f,
                                                              .max_depth_bound = 1.0f,
                                                              .stencil_test_enabled = false},
                    .multisample_state_info =
                        GraphicsPipelineMultisampleStateInfo{.multisample_enabled = true,
                                                             .sample_count =
                                                                 SampleCount::SampleCount_4,
                                                             .sample_mask = {},
                                                             .sample_shading_enabled = false,
                                                             .min_sample_shading = 0.0f,
                                                             .alpha_to_coverage_enabled = false,
                                                             .alpha_to_one_enabled = false},
                    .rasterization_state_info =
                        GraphicsPipelineRasterizationStateInfo{.depth_clamp_enabled = false,
                                                               .rasterizer_discard_enabled = false,
                                                               .polygon_mode = PolygonMode::Fill,
                                                               .cull_mode = CullMode::None,
                                                               .front_face = FrontFace::Clockwise,
                                                               .line_width = 1.0f},
                    .viewport_state_info =
                        GraphicsPipelineViewportStateInfo{.viewport_enabled = false}}};

            pipeline = Pipeline(ctx, pipeline_info);
        }

        UniformData uniform_data;

        const BufferInfo uniform_buffer_info = {.size = sizeof(uniform_data),
                                                .flags = BufferFlagBits::MapWrite |
                                                         BufferFlagBits::PersistentMapping};
        Buffer uniform_buffer(
            ctx,
            uniform_buffer_info,
            BufferInitInfo{.init_data = reinterpret_cast<std::byte*>(&uniform_data)});
        std::byte* uniform_data_map_ptr = uniform_buffer.Map();

        const BufferInfo cubemap_uniform_buffer_info = {.size = sizeof(uniform_data),
                                                        .flags = BufferFlagBits::MapWrite |
                                                                 BufferFlagBits::PersistentMapping};
        Buffer cubemap_uniform_buffer(
            ctx,
            cubemap_uniform_buffer_info,
            BufferInitInfo{.init_data = reinterpret_cast<std::byte*>(&uniform_data)});
        std::byte* cubemap_uniform_data_map_ptr = cubemap_uniform_buffer.Map();

        struct MeshData
        {
            Buffer vertex_buffer;
            Buffer index_buffer;
            std::uint32_t count;
        };

        std::vector<MeshData> meshes;
        meshes.reserve(obj_meshses.size());
        for(auto& obj_mesh: obj_meshses)
        {
            const BufferInfo vertex_buffer_info = {
                .size = static_cast<GLsizeiptr>(obj_mesh.vertex_data.size() * sizeof(float)),
                .flags = 0};

            Buffer vertex_buffer(ctx,
                                 vertex_buffer_info,
                                 BufferInitInfo{.init_data = reinterpret_cast<const std::byte*>(
                                                    obj_mesh.vertex_data.data())});

            const BufferInfo index_buffer_info = {
                .size = static_cast<GLsizeiptr>(obj_mesh.index_data.size() * sizeof(std::uint32_t)),
                .flags = 0};

            Buffer index_buffer(ctx,
                                index_buffer_info,
                                BufferInitInfo{.init_data = reinterpret_cast<const std::byte*>(
                                                   obj_mesh.index_data.data())});

            meshes.push_back(
                {MeshData{.vertex_buffer = std::move(vertex_buffer),
                          .index_buffer = std::move(index_buffer),
                          .count = static_cast<uint32_t>(obj_mesh.index_data.size())}});
        }

        const ImageInfo color_texture_info = {
            .image_type = ImageType::Image2D,
            .format = Format::R8G8B8A8_SNORM,
            .extent = Extent3D{.width = width, .height = height, .depth = 1},
            .mip_levels = 1,
            .array_layers = 1,
            .samples = SampleCount::SampleCount_4};

        Image color_texture(ctx, color_texture_info, {});

        const ImageInfo depth_texture_info = {
            .image_type = ImageType::Image2D,
            .format = Format::D24_UNORM_S8_UINT,
            .extent = Extent3D{.width = width, .height = height, .depth = 1},
            .mip_levels = 1,
            .array_layers = 1,
            .samples = SampleCount::SampleCount_4};

        Image depth_texture(ctx, depth_texture_info, {});

        AttachmentRef color_texture_ref = {.attachment = &color_texture};
        AttachmentRef depth_texture_ref = {.attachment = &depth_texture};
        const FramebufferInfo framebuffer_info = {.color_attachments = {&color_texture_ref, 1},
                                                  .depth_stencil_attachment = &depth_texture_ref};

        Framebuffer framebuffer(ctx, framebuffer_info);

        AttachmentDescription color_attachment = {.clear_load = true};
        AttachmentDescription depth_attachment = {.clear_load = true};
        const RenderPassInfo rpass_info = {.color_attachment_descriptions = {&color_attachment, 1},
                                           .depth_stencil_attachment_description =
                                               &depth_attachment};

        RenderPass renderpass(ctx, rpass_info);

        Pipeline blit_pipeline;
        {
            auto vertex_shader_data = ReadFile(GAMEDATA_PREFIX / "blit.vert");
            auto fragment_shader_data = ReadFile(GAMEDATA_PREFIX / "blit.frag");

            const ShaderInfo vertex_shader_info = {
                .stage = ShaderStage::Vertex,
                .code = vertex_shader_data.data(),
                .code_size = static_cast<GLint>(vertex_shader_data.size())};
            Shader vertex_shader(ctx, vertex_shader_info);

            const ShaderInfo fragment_shader_info = {
                .stage = ShaderStage::Fragment,
                .code = fragment_shader_data.data(),
                .code_size = static_cast<GLint>(fragment_shader_data.size())};
            Shader fragment_shader(ctx, fragment_shader_info);

            std::array shaders{&vertex_shader, &fragment_shader};

            const GraphicsPipelineInfo pipeline_info = {
                .shaders = shaders,
                .state_info = GraphicsPipelineStateInfo{
                    .vertex_input_state_info =
                        GraphicsPipelineVertexInputStateInfo{.vertex_input_bindings = {},
                                                             .vertex_input_attributes = {}},
                    .input_assembly_state_info =
                        GraphicsPipelineInputAssemblyStateInfo{.topology =
                                                                   PrimitiveTopology::Triangles,
                                                               .primitive_restart_enabled = false},
                    .blend_state_info = GraphicsPipelineBlendStateInfo{.blend_enabled = false},
                    .depth_stencil_state_info =
                        GraphicsPipelineDepthStencilStateInfo{.depth_test_enabled = false,
                                                              .stencil_test_enabled = false},
                    .multisample_state_info =
                        GraphicsPipelineMultisampleStateInfo{.multisample_enabled = false},
                    .rasterization_state_info =
                        GraphicsPipelineRasterizationStateInfo{.depth_clamp_enabled = false,
                                                               .rasterizer_discard_enabled = false,
                                                               .polygon_mode = PolygonMode::Fill,
                                                               .cull_mode = CullMode::None,
                                                               .front_face = FrontFace::Clockwise,
                                                               .line_width = 1.0f},
                    .viewport_state_info =
                        GraphicsPipelineViewportStateInfo{.viewport_enabled = false}}};

            blit_pipeline = Pipeline(ctx, pipeline_info);
        }

        AttachmentDescription blit_color_attachment = {.clear_load = false};
        const RenderPassInfo blit_rpass_info = {
            .color_attachment_descriptions = {&blit_color_attachment, 1},
            .depth_stencil_attachment_description = nullptr};

        RenderPass blit_renderpass(ctx, blit_rpass_info);

        Pipeline cubemap_pipeline;
        {
            auto vertex_shader_data = ReadFile(GAMEDATA_PREFIX / "cubemap.vert");
            auto fragment_shader_data = ReadFile(GAMEDATA_PREFIX / "cubemap.frag");

            const ShaderInfo vertex_shader_info = {
                .stage = ShaderStage::Vertex,
                .code = vertex_shader_data.data(),
                .code_size = static_cast<GLint>(vertex_shader_data.size())};
            Shader vertex_shader(ctx, vertex_shader_info);

            const ShaderInfo fragment_shader_info = {
                .stage = ShaderStage::Fragment,
                .code = fragment_shader_data.data(),
                .code_size = static_cast<GLint>(fragment_shader_data.size())};
            Shader fragment_shader(ctx, fragment_shader_info);

            std::array shaders{&vertex_shader, &fragment_shader};

            const GraphicsPipelineInfo pipeline_info = {
                .shaders = shaders,
                .state_info = GraphicsPipelineStateInfo{
                    .vertex_input_state_info =
                        GraphicsPipelineVertexInputStateInfo{.vertex_input_bindings = {},
                                                             .vertex_input_attributes = {}},
                    .input_assembly_state_info =
                        GraphicsPipelineInputAssemblyStateInfo{.topology =
                                                                   PrimitiveTopology::Triangles,
                                                               .primitive_restart_enabled = false},
                    .blend_state_info = GraphicsPipelineBlendStateInfo{.blend_enabled = false},
                    .depth_stencil_state_info =
                        GraphicsPipelineDepthStencilStateInfo{.depth_test_enabled = false,
                                                              .stencil_test_enabled = false},
                    .multisample_state_info =
                        GraphicsPipelineMultisampleStateInfo{.multisample_enabled = false},
                    .rasterization_state_info =
                        GraphicsPipelineRasterizationStateInfo{.depth_clamp_enabled = false,
                                                               .rasterizer_discard_enabled = false,
                                                               .polygon_mode = PolygonMode::Fill,
                                                               .cull_mode = CullMode::None,
                                                               .front_face = FrontFace::Clockwise,
                                                               .line_width = 1.0f},
                    .viewport_state_info =
                        GraphicsPipelineViewportStateInfo{.viewport_enabled = false}}};

            cubemap_pipeline = Pipeline(ctx, pipeline_info);
        }

        bool is_run = true;

        view_translate = hrs::math::glsl::translate(hrs::math::glsl::vec3{0, 0, 0});

        static bool in_game = false;

        std::uint64_t frame = 0;

        std::chrono::duration<float, std::milli> timer_stride(0);
        std::chrono::duration<float, std::milli> day_time_timer(0);

        Queue transfer_queue = ctx->GetQueue(QueueSpecialization::Transfer);
        Queue render_queue = ctx->GetQueue(QueueSpecialization::Graphics);

        std::vector<Fence> fences;
        fences.reserve(ctx->GetResourceSetCount());
        for(std::size_t i = 0; i < ctx->GetResourceSetCount(); i++)
        {
            fences.push_back(Fence(ctx));
        }

        //transfer + acquire image -> render -> release

        std::vector<Semaphore> transfer_signal_semaphores; //signal transfer, wait render
        transfer_signal_semaphores.reserve(ctx->GetResourceSetCount());
        for(std::size_t i = 0; i < ctx->GetResourceSetCount(); i++)
        {
            transfer_signal_semaphores.push_back(Semaphore(ctx));
        }

        std::vector<Semaphore> acquire_image_signal_semaphores; //signal acquire, wait render
        acquire_image_signal_semaphores.reserve(ctx->GetResourceSetCount());
        for(std::size_t i = 0; i < ctx->GetResourceSetCount(); i++)
        {
            acquire_image_signal_semaphores.push_back(Semaphore(ctx));
        }

        std::vector<Semaphore> render_signal_semaphores; //signal render, wait release
        render_signal_semaphores.reserve(ctx->GetResourceSetCount());
        for(std::size_t i = 0; i < ctx->GetResourceSetCount(); i++)
        {
            render_signal_semaphores.push_back(Semaphore(ctx));
        }

        while(is_run)
        {
            auto timer_start = std::chrono::system_clock::now();

            ctx->AcquireNextResourceSet();

            fences[ctx->GetCurrentResourceSetIndex()].Wait(
                std::numeric_limits<std::uint64_t>::max());

            QueueBeginInfo transfer_queue_begin_info = {.wait_seamphores = {}};
            transfer_queue.Begin(transfer_queue_begin_info);

            SDL_Event event;
            while(SDL_PollEvent(&event))
            {
                switch(event.type)
                {
                    case SDL_EventType::SDL_WINDOWEVENT:
                        switch(event.window.event)
                        {
                            case SDL_WindowEventID::SDL_WINDOWEVENT_CLOSE:
                                is_run = false;
                                break;
                            case SDL_WindowEventID::SDL_WINDOWEVENT_RESIZED:
                                width = event.window.data1;
                                height = event.window.data2;
                                break;
                        }
                        break;
                    case SDL_EventType::SDL_KEYDOWN:
                    {
                        switch(event.key.keysym.sym)
                        {
                            case SDLK_ESCAPE:
                                in_game = !in_game;
                                SDL_SetRelativeMouseMode(in_game ? SDL_TRUE : SDL_FALSE);
                                break;
                            case SDLK_1:
                                window->SetFullscreenState(WindowFullscreenState::Fullscreen);
                                break;
                            case SDLK_2:
                                window->SetFullscreenState(WindowFullscreenState::Desktop);
                                break;
                            case SDLK_3:
                                window->SetFullscreenState(WindowFullscreenState::Windowed);
                                break;
                            case SDLK_4:
                                window->SetPresentMode(WindowPresentMode::Immediate);
                                break;
                            case SDLK_5:
                                window->SetPresentMode(WindowPresentMode::VSync);
                                break;
                            case SDLK_6:
                                window->SetPresentMode(WindowPresentMode::AdaptiveVSync);
                                break;
                        }
                    }
                    break;
                    case SDL_EventType::SDL_MOUSEMOTION:
                    {
                        hrs::math::glsl::ivec2 delta =
                            hrs::math::glsl::ivec2{event.motion.xrel, event.motion.yrel};
                        if(in_game)
                        {
                            if(!(delta[0] == 0 && delta[1] == 0))
                            {
                                static float view_rotate_x_angle = 0.0f;
                                view_rotate_x_angle += -delta[0] * 0.001f;

                                static float view_rotate_y_angle = 0.0f;
                                view_rotate_y_angle += -delta[1] * 0.001f;

                                auto view_rotate_x = hrs::math::quat(hrs::math::glsl::vec3{0, 1, 0},
                                                                     view_rotate_x_angle);

                                auto view_rotate_x_mat = view_rotate_x.to_matrix();

                                auto view_rotate_y =
                                    hrs::math::quat(view_rotate_x_mat[0], view_rotate_y_angle);

                                auto view_rotate_y_mat = view_rotate_y.to_matrix();

                                update_view = true;

                                view_rotate = view_rotate_x_mat * view_rotate_y_mat;
                            }
                        }
                    }
                    break;
                }
            }

            auto state = SDL_GetKeyboardState(nullptr);
            if(state[SDL_GetScancodeFromKey(SDLK_w)])
            {
                view_translate[3] += hrs::math::glsl::vec3(view_rotate[2] * 0.001f);
                update_view = true;
            }
            else if(state[SDL_GetScancodeFromKey(SDLK_s)])
            {
                view_translate[3] += hrs::math::glsl::vec3(view_rotate[2] * -0.001f);
                update_view = true;
            }
            else if(state[SDL_GetScancodeFromKey(SDLK_a)])
            {
                view_translate[3] += hrs::math::glsl::vec3(view_rotate[0] * -0.001f);
                update_view = true;
            }
            else if(state[SDL_GetScancodeFromKey(SDLK_d)])
            {
                view_translate[3] += hrs::math::glsl::vec3(view_rotate[0] * 0.001f);
                update_view = true;
            }

            if(update_view)
            {
                auto view_translated_tmp = view_translate;
                view_translated_tmp[3][0] = -view_translate[3][0];
                view_translated_tmp[3][1] = -view_translate[3][1];
                view_translated_tmp[3][2] = -view_translate[3][2];

                auto view_rotate_tmp = view_rotate.transpose();

                view = view_translated_tmp * view_rotate_tmp;
                std::memcpy(uniform_data_map_ptr + sizeof(hrs::math::glsl::std140::mat4x4),
                            &view,
                            sizeof(hrs::math::glsl::std140::mat4x4));

                std::memcpy(cubemap_uniform_data_map_ptr + sizeof(hrs::math::glsl::std140::mat4x4),
                            &view_rotate_tmp,
                            sizeof(hrs::math::glsl::std140::mat4x4));
                update_view = false;
            }

            auto transfer_signal_sem =
                &transfer_signal_semaphores[ctx->GetCurrentResourceSetIndex()];
            QueueFlushInfo transfer_queue_flush_info = {
                .signal_fence = nullptr,
                .signal_seamphores = {&transfer_signal_sem, 1}};
            transfer_queue.Flush(transfer_queue_flush_info);

            auto acquire_image_signal_sem =
                &acquire_image_signal_semaphores[ctx->GetCurrentResourceSetIndex()];
            ctx->AcquireNextSwapchainImage(acquire_image_signal_sem);

            std::array render_wait_sems = {transfer_signal_sem, acquire_image_signal_sem};
            QueueBeginInfo render_queue_begin_info = {.wait_seamphores = render_wait_sems};
            render_queue.Begin(render_queue_begin_info);

            Framebuffer* default_fb = ctx->GetCurrentDefaultFramebuffer();

            ClearColorValue clear_color = {.value = ClearColorFloatValue{1.0f, 0.0f, 0.0f, 0.0f}};
            const RenderPassBeginInfo begin_info = {
                .framebuffer = &framebuffer,
                .clear_color_values = {&clear_color, 1},
                .clear_depth_stencil_value = ClearDepthStencilValue{.depth = 1.0f, .stencil = 0}};

            renderpass.Begin(begin_info);
            {
                const Viewport viewport = {.x = 0,
                                           .y = 0,
                                           .width = static_cast<float>(width),
                                           .height = static_cast<float>(height)};

                cubemap_pipeline.Bind();
                {
                    cubemap_pipeline.SetViewport(0, {&viewport, 1});
                    const BufferBindDesc bind_desc = {.index = 0,
                                                      .offset = 0,
                                                      .size = sizeof(uniform_data)};

                    cubemap_uniform_buffer.BindUniformBuffer(bind_desc);

                    constexpr std::size_t TIMER_QUANT = 5000; //one second
                    float day_time = day_time_timer.count() / TIMER_QUANT;

                    float lerp_t = (day_time - std::floor(day_time));
                    std::size_t first_cubemap_index =
                        static_cast<std::size_t>(std::floor(day_time)) %
                        cubemap_order_handles.size();
                    std::size_t second_cubemap_index =
                        (first_cubemap_index + 1) % cubemap_order_handles.size();

                    cubemap_order_handles[first_cubemap_index]->image_view.BindTextureView(0);
                    cubemap_order_handles[second_cubemap_index]->image_view.BindTextureView(1);

                    cubemap_pipeline.SetUniform(0, {&lerp_t, 1});

                    //view_cubemap.BindTextureView(0);
                    cubemap_sampler.BindSampler(0);
                    cubemap_sampler.BindSampler(1);

                    cubemap_pipeline.Draw(36, 1, 0, 0);
                }

                pipeline.Bind();
                {
                    pipeline.SetViewport(0, {&viewport, 1});

                    const BufferBindDesc bind_desc = {.index = 0,
                                                      .offset = 0,
                                                      .size = sizeof(uniform_data)};

                    uniform_buffer.BindUniformBuffer(bind_desc);
                    for(const auto& mesh: meshes)
                    {
                        pipeline.BindVertexBuffer(mesh.vertex_buffer, 0, 0);
                        pipeline.BindIndexBuffer(mesh.index_buffer, IndexType::u32, 0);

                        pipeline.DrawIndexed(mesh.count, 1, 0, 0, 0);
                    }
                }
            }
            renderpass.End();

            const RenderPassBeginInfo blit_begin_info = {.framebuffer = default_fb,
                                                         .clear_color_values = {},
                                                         .clear_depth_stencil_value = {}};
            blit_renderpass.Begin(blit_begin_info);
            {
                blit_pipeline.Bind();
                const Viewport viewport = {.x = 0,
                                           .y = 0,
                                           .width = static_cast<float>(width),
                                           .height = static_cast<float>(height)};
                blit_pipeline.SetViewport(0, {&viewport, 1});

                color_texture.BindTexture(0);
                pipeline.Draw(6, 1, 0, 0);
            }
            blit_renderpass.End();

            auto render_signal_sem = &render_signal_semaphores[ctx->GetNextResourceSetIndex()];
            QueueFlushInfo render_queue_flush_info = {
                .signal_fence = &fences[ctx->GetCurrentResourceSetIndex()],
                .signal_seamphores = {&render_signal_sem, 1}};
            render_queue.Flush(render_queue_flush_info);

            PresentInfo present_info = {.wait_semaphores = {&render_signal_sem, 1}};
            ctx->ReleaseSwapchainImage(present_info);

            ctx->ReleaseCurrentResourceSet();

            GLenum gl_err = ctx->GetLoader().GetError();
            if(gl_err != GL_NO_ERROR)
                throw std::runtime_error(std::to_string(gl_err));

            auto timer_end = std::chrono::system_clock::now();

            std::chrono::duration<float, std::milli> timer_dur = timer_end - timer_start;

            day_time_timer += timer_dur;

            timer_stride += timer_dur;
            if(timer_stride > decltype(timer_stride)(100))
            {
                float fps = 1000.0f / timer_dur.count();
                auto title =
                    std::format("FPS = {}; TIME = {}; FRAME = {}", fps, timer_dur.count(), frame);
                window->SetTitle(title.c_str());

                timer_stride = decltype(timer_stride)(0);
            }

            frame++;
        }
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "Unhandled exception" << std::endl;
    }

    return 0;
}

#include "Resolve.h"
#include "Context.h"
#include <bit>
#include <stdexcept>
#include "Core/Utils/Binary.hpp"
#include "hrs/scoped_call.hpp"
#include "glad/wgl.h"

namespace OpenGL
{
    static std::string WIN32_ERROR;
    static Render::ContextInitProperties context_init_properties;
    static std::vector<std::size_t> swapchain_config_indices_mapping;

#define WIN32_RETURN_WND_PROC_ERROR(MSG, ...) \
    WIN32_ERROR = MSG __VA_OPT__(, __VA_ARGS__); \
    return -1;

    static GLADapiproc GetProcAddressWIN32(const char* name)
    {
        return reinterpret_cast<GLADapiproc>(wglGetProcAddress(name));
    }

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

    template<typename T>
    static T get_property(GladGLContext& loader, GLenum name) noexcept
    {
        if constexpr(std::same_as<std::uint32_t, T> || std::same_as<std::int32_t, T>)
        {
            GLint value;
            loader.GetIntegerv(name, &value);

            return static_cast<T>(value);
        }
        else if constexpr(std::same_as<std::uint64_t, T> || std::same_as<std::int64_t, T>)
        {
            GLint64 value;
            loader.GetInteger64v(name, &value);

            return static_cast<T>(value);
        }
        else if constexpr(std::same_as<float, T>)
        {
            float value;
            loader.GetFloatv(name, &value);

            return value;
        }
        else if constexpr(std::same_as<Render::ComputeGroupSize, T>)
        {
            GLint values[3];
            loader.GetIntegeri_v(name, 0, values + 0);
            loader.GetIntegeri_v(name, 1, values + 1);
            loader.GetIntegeri_v(name, 2, values + 2);

            return Render::ComputeGroupSize{.x = static_cast<std::uint32_t>(values[0]),
                                            .y = static_cast<std::uint32_t>(values[1]),
                                            .z = static_cast<std::uint32_t>(values[2])};
        }
        else if constexpr(std::same_as<Render::Extent2D, T>)
        {
            GLfloat values[2];
            loader.GetFloatv(name, values);

            return Render::Extent2D{.width = static_cast<std::uint32_t>(values[0]),
                                    .height = static_cast<std::uint32_t>(values[1])};
        }
        else if constexpr(std::same_as<Render::Range, T>)
        {
            GLfloat values[2];
            loader.GetFloatv(name, values);

            return Render::Range{.min = values[0], .max = values[1]};
        }
        else if constexpr(std::same_as<Render::SampleCountFlags, T>)
        {
            GLint value;
            loader.GetIntegerv(name, &value);

            static_assert(std::is_signed_v<Render::SampleCountFlags> &&
                          sizeof(Render::SampleCountFlags) == 4);

            std::uint32_t u_value = static_cast<std::uint32_t>(value);
            if(!Core::IsPowerOf2(u_value))
                value = std::bit_floor(u_value);

            Render::SampleCountFlags samples = 0;
            while(!(samples & u_value))
            {
                samples |= 0x1;
                samples <<= 1;
            }

            return samples;
        }
        else
            assert(false);
    }

    template<typename T, typename... I>
    static T get_property_min(GladGLContext& loader, I... names) noexcept
    {
        if constexpr(std::same_as<std::uint32_t, T> || std::same_as<std::int32_t, T>)
        {
            return std::min({(get_property<T>(loader, names), ...)});
        }
        else
            assert(false);
    }

    static void FillContextProperties()
    {
        GladGLContext loader;
        int glad_ver =
            gladLoadGLContext(&loader, reinterpret_cast<GLADloadfunc>(GetProcAddressWIN32));
        if(glad_ver == 0)
            throw std::runtime_error("Failed to load GLAD");

        GLint extensions_number = 0;
        loader.GetIntegerv(GL_NUM_EXTENSIONS, &extensions_number);

        std::vector<std::string> extensions;

        extensions.resize(extensions_number);
        for(std::size_t i = 0; i < extensions_number; i++)
            extensions[i] = reinterpret_cast<const char*>(loader.GetStringi(GL_EXTENSIONS, i));

        GLint major = 0;
        GLint minor = 0;
        loader.GetIntegerv(GL_MAJOR_VERSION, &major);
        loader.GetIntegerv(GL_MINOR_VERSION, &minor);

        //properties:
        std::uint32_t max_uniform_size =
            get_property_min<std::uint32_t>(loader,
                                            GL_MAX_UNIFORM_LOCATIONS,
                                            GL_MAX_COMPUTE_UNIFORM_COMPONENTS,
                                            GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
                                            GL_MAX_GEOMETRY_UNIFORM_COMPONENTS,
                                            GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS,
                                            GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS,
                                            GL_MAX_VERTEX_UNIFORM_COMPONENTS) *
            4;

        std::uint32_t max_per_stage_descriptor_textures =
            get_property_min<std::uint32_t>(loader,
                                            GL_MAX_TEXTURE_IMAGE_UNITS,
                                            GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
                                            GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS,
                                            GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS,
                                            GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS,
                                            GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS,
                                            GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);

        std::uint32_t max_per_stage_descriptor_uniform_buffers =
            get_property_min<std::uint32_t>(loader,
                                            GL_MAX_COMBINED_UNIFORM_BLOCKS,
                                            GL_MAX_COMPUTE_UNIFORM_BLOCKS,
                                            GL_MAX_FRAGMENT_UNIFORM_BLOCKS,
                                            GL_MAX_GEOMETRY_UNIFORM_BLOCKS,
                                            GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS,
                                            GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS,
                                            GL_MAX_VERTEX_UNIFORM_BLOCKS);

        std::uint32_t max_per_stage_descriptor_storage_buffers =
            get_property_min<std::uint32_t>(loader,
                                            GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS,
                                            GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS,
                                            GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS,
                                            GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS,
                                            GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,
                                            GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS,
                                            GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS);

        std::uint32_t max_per_stage_descriptor_storage_images = get_property_min<std::uint32_t>(
            loader,
            max_per_stage_descriptor_textures, //because texel buffers in VK are storage images but in OGL they are textures...
            GL_MAX_COMBINED_IMAGE_UNIFORMS,
            GL_MAX_COMPUTE_IMAGE_UNIFORMS,
            GL_MAX_FRAGMENT_IMAGE_UNIFORMS,
            GL_MAX_GEOMETRY_IMAGE_UNIFORMS,
            GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS,
            GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS,
            GL_MAX_VERTEX_IMAGE_UNIFORMS);

        context_init_properties.properties = Render::ContextProperties{
            .context_name = "OpenGL",
            .supported_backend_type = Core::RenderBackendType::OpenGL,
            .version = Render::MakeVersion(major, minor),
            .vendor_name = reinterpret_cast<const char*>(loader.GetString(GL_VENDOR)),
            .device_name = reinterpret_cast<const char*>(loader.GetString(GL_RENDERER)),
            .extensions = std::move(extensions),
            .supported_syntax = Render::ShaderSyntaxFlagBits::GLSL,
            .queue_family_properties = {Render::QueueFamilyProperties{
                .specialization = Render::QueueSpecializationFlagBits::TransferSpec |
                                  Render::QueueSpecializationFlagBits::ComputeSpec |
                                  Render::QueueSpecializationFlagBits::GraphicsSpec |
                                  Render::QueueSpecializationFlagBits::PresentSpec,
                .queue_count = 1}},
            .memory_types = create_memory_types(),
            .command_buffer_strategy = Render::CommandBufferStrategy::Immediate,
            .device_type = Render::ContextDeviceType::Other,
            .view_origin = Render::ViewOrigin::BottomLeft,
            .clip_space_depth_bounds =
                Render::ClipSpaceDepthBounds{.min = 0.0f,
                                             .max = 1.0f}, //due to glClipControl(GL_ZERO_TO_ONE)
            .limits =
                Render::ContextLimits{
                    .max_image_dimension_1D =
                        get_property<std::uint32_t>(loader, GL_MAX_TEXTURE_SIZE),
                    .max_image_dimension_2D =
                        get_property<std::uint32_t>(loader, GL_MAX_TEXTURE_SIZE),
                    .max_image_dimension_3D =
                        get_property<std::uint32_t>(loader, GL_MAX_3D_TEXTURE_SIZE),
                    .max_image_dimension_cube =
                        get_property<std::uint32_t>(loader, GL_MAX_CUBE_MAP_TEXTURE_SIZE),
                    .max_image_array_layers =
                        get_property<std::uint32_t>(loader, GL_MAX_ARRAY_TEXTURE_LAYERS),
                    .max_texel_buffer_elements =
                        get_property<std::uint32_t>(loader, GL_MAX_TEXTURE_BUFFER_SIZE),
                    .max_uniform_buffer_range =
                        get_property<std::uint32_t>(loader, GL_MAX_UNIFORM_BLOCK_SIZE),
                    .max_storage_buffer_range =
                        get_property<std::uint64_t>(loader, GL_MAX_SHADER_STORAGE_BLOCK_SIZE),
                    .max_uniform_size = max_uniform_size,
                    .max_sampler_allocation_count = std::numeric_limits<std::uint32_t>::max(),
                    .max_bound_descriptor_sets = std::numeric_limits<std::uint32_t>::max(),
                    .max_per_stage_descriptor_samplers = max_per_stage_descriptor_textures,
                    .max_per_stage_descriptor_uniform_buffers =
                        max_per_stage_descriptor_uniform_buffers,
                    .max_per_stage_descriptor_storage_buffers =
                        max_per_stage_descriptor_storage_buffers,
                    .max_per_stage_descriptor_sampled_images = max_per_stage_descriptor_textures,
                    .max_per_stage_descriptor_storage_images =
                        max_per_stage_descriptor_storage_images,
                    .max_per_stage_descriptor_input_attachments = max_per_stage_descriptor_textures,
                    .max_per_stage_resources = max_per_stage_descriptor_textures +
                                               max_per_stage_descriptor_uniform_buffers +
                                               max_per_stage_descriptor_storage_buffers +
                                               max_per_stage_descriptor_storage_images,
                    .max_descriptor_set_samplers = std::numeric_limits<std::uint32_t>::max(),
                    .max_descriptor_set_uniform_buffers = std::numeric_limits<std::uint32_t>::max(),
                    .max_descriptor_set_storage_buffers = std::numeric_limits<std::uint32_t>::max(),
                    .max_descriptor_set_sampled_images = std::numeric_limits<std::uint32_t>::max(),
                    .max_descriptor_set_storage_images = std::numeric_limits<std::uint32_t>::max(),
                    .max_descriptor_set_input_attachments =
                        std::numeric_limits<std::uint32_t>::max(),
                    .max_vertex_input_attributes =
                        get_property<std::uint32_t>(loader, GL_MAX_VERTEX_ATTRIBS),
                    .max_vertex_input_bindings =
                        get_property<std::uint32_t>(loader, GL_MAX_VERTEX_ATTRIB_BINDINGS),
                    .max_vertex_input_attribute_offset =
                        get_property<std::uint32_t>(loader, GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET),
                    .max_vertex_input_binding_stride =
                        get_property<std::uint32_t>(loader, GL_MAX_VERTEX_ATTRIB_STRIDE),
                    .max_vertex_output_components =
                        get_property<std::uint32_t>(loader, GL_MAX_VERTEX_OUTPUT_COMPONENTS),
                    .max_tessellation_generation_level =
                        get_property<std::uint32_t>(loader, GL_MAX_TESS_GEN_LEVEL),
                    .max_tessellation_patch_size =
                        get_property<std::uint32_t>(loader, GL_MAX_PATCH_VERTICES),
                    .max_tessellation_control_per_vertex_input_components =
                        get_property<std::uint32_t>(loader, GL_MAX_TESS_CONTROL_INPUT_COMPONENTS),
                    .max_tessellation_control_per_vertex_output_components =
                        get_property<std::uint32_t>(loader, GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS),
                    .max_tessellation_control_per_patch_output_components =
                        get_property<std::uint32_t>(loader, GL_MAX_TESS_PATCH_COMPONENTS),
                    .max_tessellation_control_total_output_components =
                        get_property<std::uint32_t>(loader,
                                                    GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS),
                    .max_tessellation_evaluation_input_components =
                        get_property<std::uint32_t>(loader,
                                                    GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS),
                    .max_tessellation_evaluation_output_components =
                        get_property<std::uint32_t>(loader,
                                                    GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS),
                    .max_geometry_shader_invocations =
                        get_property<std::uint32_t>(loader, GL_MAX_GEOMETRY_SHADER_INVOCATIONS),
                    .max_geometry_input_components =
                        get_property<std::uint32_t>(loader, GL_MAX_GEOMETRY_INPUT_COMPONENTS),
                    .max_geometry_output_components =
                        get_property<std::uint32_t>(loader, GL_MAX_GEOMETRY_OUTPUT_COMPONENTS),
                    .max_geometry_output_vertices =
                        get_property<std::uint32_t>(loader, GL_MAX_GEOMETRY_OUTPUT_VERTICES),
                    .max_geometry_total_output_components =
                        get_property<std::uint32_t>(loader,
                                                    GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS),
                    .max_fragment_input_components =
                        get_property<std::uint32_t>(loader, GL_MAX_FRAGMENT_INPUT_COMPONENTS),
                    .max_fragment_output_attachments =
                        get_property<std::uint32_t>(loader, GL_MAX_DRAW_BUFFERS),
                    .max_fragment_dual_src_attachments =
                        get_property<std::uint32_t>(loader, GL_MAX_DUAL_SOURCE_DRAW_BUFFERS),
                    .max_fragment_combined_output_resources =
                        get_property<std::uint32_t>(loader,
                                                    GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES),
                    .max_compute_shared_memory_size =
                        get_property<std::uint32_t>(loader, GL_MAX_COMPUTE_SHARED_MEMORY_SIZE),
                    .max_compute_work_group_count =
                        get_property<Render::ComputeGroupSize>(
                            loader,
                            GL_MAX_COMPUTE_WORK_GROUP_COUNT),
                    .max_compute_work_group_invocations =
                        get_property<std::uint32_t>(loader, GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS),
                    .max_compute_work_group_size =
                        get_property<Render::ComputeGroupSize>(
                            loader,
                            GL_MAX_COMPUTE_WORK_GROUP_SIZE),
                    .sub_pixel_precision_bits =
                        get_property<std::uint32_t>(loader, GL_SUBPIXEL_BITS),
                    .max_draw_indexed_index_value =
                        std::numeric_limits<std::uint32_t>::max() - 1 /*GL_MAX_ELEMENT_INDEX*/,
                    .max_draw_indirect_count = std::numeric_limits<std::uint32_t>::max(),
                    .max_sampler_lod_bias = get_property<float>(loader, GL_MAX_TEXTURE_LOD_BIAS),
                    .max_sampler_anisotropy =
                        ((loader.VERSION_4_6 || loader.ARB_texture_filter_anisotropic) ?
                             get_property<float>(loader, GL_MAX_TEXTURE_MAX_ANISOTROPY) :
                             1.0f),
                    .max_viewports = get_property<std::uint32_t>(loader, GL_MAX_VIEWPORTS),
                    .max_viewport_dimensions =
                        get_property<Render::Extent2D>(loader, GL_MAX_VIEWPORT_DIMS),
                    .viewport_bounds_range =
                        get_property<Render::Range>(loader, GL_VIEWPORT_BOUNDS_RANGE),
                    .viewport_sub_pixel_bits =
                        get_property<std::uint32_t>(loader, GL_VIEWPORT_SUBPIXEL_BITS),
                    .min_memory_map_alignment =
                        get_property<std::uint32_t>(loader, GL_MIN_MAP_BUFFER_ALIGNMENT),
                    .min_texel_buffer_offset_alignment =
                        get_property<std::uint32_t>(loader, GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT),
                    .min_uniform_buffer_offset_alignment =
                        get_property<std::uint32_t>(loader, GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT),
                    .min_storage_buffer_offset_alignment =
                        get_property<std::uint32_t>(loader,
                                                    GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT),
                    .min_texel_offset =
                        get_property<std::int32_t>(loader, GL_MIN_PROGRAM_TEXEL_OFFSET),
                    .max_texel_offset =
                        get_property<std::uint32_t>(loader, GL_MAX_PROGRAM_TEXEL_OFFSET),
                    .min_texel_gather_offset =
                        get_property<std::int32_t>(loader, GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET),
                    .max_texel_gather_offset =
                        get_property<std::uint32_t>(loader, GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET),
                    .min_interpolation_offset =
                        get_property<float>(loader, GL_MIN_FRAGMENT_INTERPOLATION_OFFSET),
                    .max_interpolation_offset =
                        get_property<float>(loader, GL_MAX_FRAGMENT_INTERPOLATION_OFFSET),
                    .sub_pixel_interpolation_offset_bits =
                        get_property<std::uint32_t>(loader, GL_FRAGMENT_INTERPOLATION_OFFSET_BITS),
                    .max_framebuffer_width =
                        get_property<std::uint32_t>(loader, GL_MAX_FRAMEBUFFER_WIDTH),
                    .max_framebuffer_height =
                        get_property<std::uint32_t>(loader, GL_MAX_FRAMEBUFFER_HEIGHT),
                    .max_framebuffer_layers =
                        get_property<std::uint32_t>(loader, GL_MAX_FRAMEBUFFER_LAYERS),
                    .framebuffer_color_sample_counts =
                        get_property<Render::SampleCountFlags>(loader, GL_MAX_FRAMEBUFFER_SAMPLES),
                    .framebuffer_depth_sample_counts =
                        get_property<Render::SampleCountFlags>(loader, GL_MAX_FRAMEBUFFER_SAMPLES),
                    .framebuffer_stencil_sample_counts =
                        get_property<Render::SampleCountFlags>(loader, GL_MAX_FRAMEBUFFER_SAMPLES),
                    .framebuffer_no_attachments_sample_counts =
                        get_property<Render::SampleCountFlags>(loader, GL_MAX_FRAMEBUFFER_SAMPLES),
                    .max_color_attachments =
                        get_property<std::uint32_t>(loader, GL_MAX_COLOR_ATTACHMENTS),
                    .sampled_image_color_sample_counts =
                        get_property<Render::SampleCountFlags>(
                            loader,
                            GL_MAX_COLOR_TEXTURE_SAMPLES),
                    .sampled_image_integer_sample_counts =
                        get_property<Render::SampleCountFlags>(loader, GL_MAX_INTEGER_SAMPLES),
                    .sampled_image_depth_sample_counts =
                        get_property<Render::SampleCountFlags>(
                            loader,
                            GL_MAX_DEPTH_TEXTURE_SAMPLES),
                    .sampled_image_stencil_sample_counts =
                        get_property<Render::SampleCountFlags>(
                            loader,
                            GL_MAX_DEPTH_TEXTURE_SAMPLES),
                    .storage_image_sample_counts =
                        get_property<Render::SampleCountFlags>(loader, GL_MAX_IMAGE_SAMPLES),
                    .max_sample_mask_words =
                        get_property<std::uint32_t>(loader, GL_MAX_SAMPLE_MASK_WORDS),
                    .max_clip_distances =
                        get_property<std::uint32_t>(loader, GL_MAX_CLIP_DISTANCES),
                    .max_cull_distances =
                        get_property<std::uint32_t>(loader, GL_MAX_CULL_DISTANCES),
                    .max_combined_clip_and_cull_distances =
                        get_property<std::uint32_t>(loader,
                                                    GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES),
                    .discrete_queue_priorities = std::numeric_limits<std::uint32_t>::max(),
                    .point_size_range = get_property<Render::Range>(loader, GL_POINT_SIZE_RANGE),
                    .line_width_range =,
                },
            .features = Render::ContextFeatures{
                .robust_buffer_access = static_cast<bool>(GLAD_WGL_ARB_create_context_robustness),
                .full_draw_index_uint32 = true,
                .image_cube_array = true,
                .independent_blend = true,
                .geometry_shader = true,
                .tessellation_shader = true,
                .sample_rate_shading = true,
                .dual_src_blend = true,
                .logic_op = true,
                .multi_draw_indirect = true,
                .draw_indirect_first_instance = true,
                .depth_clamp = true,
                .depth_bias_clamp = (loader.VERSION_4_6 || loader.ARB_polygon_offset_clamp),
                .fill_mode_non_solid = true,
                .depth_bounds = true,
                .wide_lines =
                    !(context_init_properties.properties.limits.line_width_range.min ==
                          context_init_properties.properties.limits.line_width_range.max &&
                      context_init_properties.properties.limits.line_width_range.min == 1.0f),
                .large_points =
                    !(context_init_properties.properties.limits.point_size_range.min ==
                          context_init_properties.properties.limits.point_size_range.max &&
                      context_init_properties.properties.limits.point_size_range.min == 1.0f),
                .alpha_to_one = true,
                .multi_viewport = (context_init_properties.properties.limits.max_viewports > 1),
                .sampler_anisotropy =
                    static_cast<bool>(loader.VERSION_4_6 || loader.ARB_texture_filter_anisotropic),
                .vertex_pipeline_stores_and_atomics = true,
                .fragment_stores_and_atomics = true,
                .shader_tessellation_and_geometry_point_size = true,
                .shader_image_gather_extended = true,
                .shader_storage_image_multisample = true,
                .shader_storage_image_read_without_format =
                    static_cast<bool>(loader.EXT_shader_image_load_formatted),
                .shader_storage_image_write_without_format = true,
                .shader_uniform_buffer_array_dynamic_indexing = true,
                .shader_sampled_image_array_dynamic_indexing = true,
                .shader_storage_buffer_array_dynamic_indexing = true,
                .shader_storage_image_array_dynamic_indexing = true,
                .shader_clip_distance = true,
                .shader_cull_distance = true,
                .shader_float64 = true,
                .shader_int64 = static_cast<bool>(loader.ARB_gpu_shader_int64),
                .shader_int16 = static_cast<bool>(loader.AMD_gpu_shader_int16),
                .shader_resource_min_lod = true,
                .variable_multisample_rate = true,
                .sampler_mirror_clamp_to_edge = true,
                .custom_border_colors = true,
                .custom_border_color_without_format = true,
                .validation_layer = true,
                .debug_messenger = true}};

#error GL_MAX_INTEGER_SAMPLES???
#error https://wikis.khronos.org/opengl/GLAPI/glLineWidth SMOOTH AND ALIASED LINES???
    }

    static LRESULT CALLBACK DummyWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch(uMsg)
        {
            case WM_CREATE:
            {
                PIXELFORMATDESCRIPTOR pfd = {.nSize = sizeof(PIXELFORMATDESCRIPTOR),
                                             .nVersion = 1,
                                             .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                                                        PFD_DOUBLEBUFFER,
                                             .iPixelType = PFD_TYPE_RGBA,
                                             .cColorBits = 32,
                                             .cRedBits = 0,
                                             .cRedShift = 0,
                                             .cGreenBits = 0,
                                             .cGreenShift = 0,
                                             .cBlueBits = 0,
                                             .cBlueShift = 0,
                                             .cAlphaBits = 0,
                                             .cAlphaShift = 0,
                                             .cAccumBits = 0,
                                             .cAccumRedBits = 0,
                                             .cAccumGreenBits = 0,
                                             .cAccumBlueBits = 0,
                                             .cAccumAlphaBits = 0,
                                             .cDepthBits = 0,
                                             .cStencilBits = 0,
                                             .cAuxBuffers = 0,
                                             .iLayerType = PFD_MAIN_PLANE,
                                             .bReserved = 0,
                                             .dwLayerMask = 0,
                                             .dwVisibleMask = 0,
                                             .dwDamageMask = 0};

                HDC dc = GetDC(hwnd);
                int format_index = ChoosePixelFormat(dc, &pfd);
                if(format_index == 0)
                {
                    WIN32_RETURN_WND_PROC_ERROR("Failed to select pixel format")
                }

                if(SetPixelFormat(dc, format_index, &pfd) == FALSE)
                {
                    WIN32_RETURN_WND_PROC_ERROR("Failed to set pixel format")
                }

                HGLRC glrc = wglCreateContext(dc);
                if(glrc == nullptr)
                {
                    WIN32_RETURN_WND_PROC_ERROR("Failed to create WGL dummy context")
                }

                hrs::scoped_call cleanup_dummy(
                    [&hwnd, &dc, &glrc]()
                    {
                        wglDeleteContext(glrc);
                        ReleaseDC(hwnd, dc);
                    });

                wglMakeCurrent(dc, glrc);

                int wgl_version = gladLoadWGL(dc, GetProcAddressWIN32);
                if(wgl_version == 0)
                {
                    WIN32_RETURN_WND_PROC_ERROR("Failed to load WGL context");
                }

                cleanup_dummy.drop();
                wglDeleteContext(glrc);

                /*
                GLAD_API_CALL int GLAD_WGL_VERSION_1_0;
                GLAD_API_CALL int GLAD_WGL_ARB_create_context;
                GLAD_API_CALL int GLAD_WGL_ARB_create_context_no_error;
                GLAD_API_CALL int GLAD_WGL_ARB_create_context_profile;
                GLAD_API_CALL int GLAD_WGL_ARB_create_context_robustness;
                GLAD_API_CALL int GLAD_WGL_ARB_extensions_string;
                GLAD_API_CALL int GLAD_WGL_ARB_framebuffer_sRGB;
                GLAD_API_CALL int GLAD_WGL_ARB_pixel_format;
                GLAD_API_CALL int GLAD_WGL_ARB_pixel_format_float;
                GLAD_API_CALL int GLAD_WGL_EXT_extensions_string;
                GLAD_API_CALL int GLAD_WGL_EXT_swap_control;
                GLAD_API_CALL int GLAD_WGL_EXT_swap_control_tear;
                */

                if(!(GLAD_WGL_ARB_create_context && GLAD_WGL_ARB_create_context_profile &&
                     GLAD_WGL_ARB_pixel_format))
                {
                    WIN32_RETURN_WND_PROC_ERROR(
                        "WGL core context or pixel format selection is not available");
                }

                constexpr static int format_number_query_input[1] = {WGL_NUMBER_PIXEL_FORMATS_ARB};
                int format_number_query_output[1] = {};

                std::vector<Render::SwapchainConfig> swapchain_configs;
                std::vector<std::size_t> swapchain_config_indices;
                glad_wglGetPixelFormatAttribivARB(dc,
                                                  0,
                                                  0,
                                                  1,
                                                  format_number_query_input,
                                                  format_number_query_output);
                swapchain_configs.reserve(format_number_query_output[0]);
                swapchain_config_indices.reserve(format_number_query_output[0]);

                constexpr static int format_query_input[] = {
                    WGL_DRAW_TO_WINDOW_ARB, //0
                    WGL_ACCELERATION_ARB, //1
                    WGL_SUPPORT_OPENGL_ARB, //2
                    WGL_DOUBLE_BUFFER_ARB, //3
                    WGL_PIXEL_TYPE_ARB, //4
                    WGL_COLOR_BITS_ARB, //5
                    WGL_RED_BITS_ARB, //6
                    WGL_GREEN_BITS_ARB, //7
                    WGL_BLUE_BITS_ARB, //8
                    WGL_ALPHA_BITS_ARB, //9
                    WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB //10
                };

                int format_query_output[std::size(format_query_input)] = {};

                std::size_t format_query_input_size =
                    (GLAD_WGL_ARB_framebuffer_sRGB ? std::size(format_query_input) :
                                                     std::size(format_query_input) - 1);

                for(std::size_t i = 0; i < format_number_query_output[0]; i++)
                {
                    glad_wglGetPixelFormatAttribivARB(dc,
                                                      i,
                                                      0,
                                                      format_query_input_size,
                                                      format_query_input,
                                                      format_query_output);

                    if(format_query_output[0 /*WGL_DRAW_TO_WINDOW_ARB*/] == FALSE ||
                       format_query_output[1 /*WGL_ACCELERATION_ARB*/] !=
                           WGL_FULL_ACCELERATION_ARB ||
                       format_query_output[2 /*WGL_SUPPORT_OPENGL_ARB*/] == FALSE ||
                       format_query_output[3 /*WGL_DOUBLE_BUFFER_ARB*/] == FALSE ||
                       format_query_output[4 /*WGL_PIXEL_TYPE_ARB*/] == WGL_TYPE_COLORINDEX_ARB)
                    {
                        continue;
                    }

                    swapchain_config_indices.push_back(i);

                    Render::SwapchainConfig cfg = {
                        .red_bits_size =
                            static_cast<std::uint8_t>(format_query_output[6 /*WGL_RED_BITS_ARB*/]),
                        .green_bits_size = static_cast<std::uint8_t>(
                            format_query_output[7 /*WGL_GREEN_BITS_ARB*/]),
                        .blue_bits_size =
                            static_cast<std::uint8_t>(format_query_output[8 /*WGL_BLUE_BITS_ARB*/]),
                        .alpha_bits_size = static_cast<std::uint8_t>(
                            format_query_output[9 /*WGL_ALPHA_BITS_ARB*/]),
                        .color_buffer_bits_size = static_cast<std::uint8_t>(
                            format_query_output[5 /*WGL_COLOR_BITS_ARB*/]),
                        .format_type = (format_query_output[4 /*WGL_PIXEL_TYPE_ARB*/] ==
                                                WGL_TYPE_RGBA_FLOAT_ARB ?
                                            Render::FormatType::SFLOAT :
                                            Render::FormatType::UNORM),
                        .srgb_format =
                            (GLAD_WGL_ARB_framebuffer_sRGB ?
                                 static_cast<bool>(
                                     format_query_output[10 /*WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB*/]) :
                                 false)};

                    swapchain_configs.push_back(cfg);
                }

                Render::PresentModeFlags supported_present_modes =
                    Render::PresentModeFlagBits::FIFO;
                if(GLAD_WGL_EXT_swap_control)
                    supported_present_modes |= Render::PresentModeFlagBits::Immediate;

                if(GLAD_WGL_EXT_swap_control_tear)
                    supported_present_modes |= Render::PresentModeFlagBits::RelaxedFIFO;

                swapchain_config_indices_mapping = std::move(swapchain_config_indices);
                context_init_properties.surface_capabilities = Render::ContextSurfaceCapabilities{
                    .min_image_count = 0,
                    .max_image_count = 2,
                    .supported_present_modes = supported_present_modes,
                    .supported_configs = std::move(swapchain_configs)};

                const int dummy_profile_attributes[] = {WGL_CONTEXT_MAJOR_VERSION_ARB,
                                                        4,
                                                        WGL_CONTEXT_MINOR_VERSION_ARB,
                                                        5,
                                                        WGL_CONTEXT_FLAGS_ARB,
                                                        WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                                                        WGL_CONTEXT_PROFILE_MASK_ARB,
                                                        WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                                                        0};

                auto dummy_profile_glrc =
                    glad_wglCreateContextAttribsARB(dc, nullptr, dummy_profile_attributes);
                if(dummy_profile_glrc == nullptr)
                {
                    WIN32_RETURN_WND_PROC_ERROR("Failed to create dummy profile context");
                }

                wglMakeCurrent(dc, dummy_profile_glrc);
#error GET PROPERTIES
                wglDeleteContext(glrc);

                PostQuitMessage(0);
            }
            break;
        }
        return -1;
    }

    static void LoadWGLFromDummy(const Render::ResolveWin32Info& info)
    {
        constexpr static wchar_t DUMMY_WINDOW_CLASS_NAME[] = L"DUMMY_WINDOW_CLASS";
        constexpr static wchar_t DUMMY_WINDOW_TITLE[] = L"DUMMY_WINDOW_TITLE";

        WNDCLASS window_class = {};
        window_class.lpszClassName = DUMMY_WINDOW_CLASS_NAME;
        window_class.hInstance = info.instance;
        window_class.lpfnWndProc = DummyWindowProc;
        auto register_res = RegisterClass(&window_class);
        if(register_res == 0)
            throw std::runtime_error("Failed to create dummy OpenGL window");

        HWND dummy_window = CreateWindowEx(0,
                                           DUMMY_WINDOW_CLASS_NAME,
                                           DUMMY_WINDOW_TITLE,
                                           0,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           info.window,
                                           nullptr,
                                           info.instance,
                                           nullptr);

        hrs::scoped_call cleanup(
            [&dummy_window, hinstance = info.instance]()
            {
                if(dummy_window != 0)
                    DestroyWindow(dummy_window);

                UnregisterClass(DUMMY_WINDOW_CLASS_NAME, hinstance);
            });

        if(!WIN32_ERROR.empty())
            throw std::runtime_error(WIN32_ERROR);
    }

    static void InitWIN32(const Render::ResolveWin32Info& info)
    {
        LoadWGLFromDummy(info);
    }

    extern "C" Render::Resolve* RenderResolve()
    {
        static Resolve resolve;

        return &resolve;
    }

    Resolve::Resolve()
        : ctx(nullptr)
    {}

    Resolve::~Resolve()
    {}

    std::span<const Core::RenderBackendType> Resolve::GetAvailableBackends()
    {
        constexpr static Core::RenderBackendType SUPPORTED_BACKEND =
            Core::RenderBackendType::OpenGL;

        return {&SUPPORTED_BACKEND, 1};
    }

    void Resolve::Init(Core::RenderBackend* backend)
    {
        if(ctx != nullptr)
            throw std::runtime_error("Cannot init already inited resolve object");

        if(backend->GetType() != Core::RenderBackendType::OpenGL)
            throw std::runtime_error("Bad render backend type. 'OpenGL' type was expected");

        ctx = new Context(static_cast<Core::OpenGLBackend*>(backend));
    }

    std::span<const Render::ContextProperties> Resolve::GetAvailableContexts()
    {
        static const Render::ContextProperties& props = ctx->GetProperties();

        return {&props, 1};
    }

    Render::Context* Resolve::CreateContext(const Render::SelectedContextDesc& desc)
    {
        if(desc.index != 0)
            throw std::runtime_error(
                "Bad selected context index. Implementation has only one available context");

        if(!(desc.queue_family_infos.size() == 1 && desc.queue_family_infos[0].index == 0 &&
             desc.queue_family_infos[0].queue_count == 1))
            throw std::runtime_error(
                "Bad selected context queue families. Implementation has only one queue");

        return ctx;
    }

    void Resolve::operator delete(void* ptr) noexcept
    {
        static_cast<Resolve*>(ptr)->ctx = nullptr;
    }
};
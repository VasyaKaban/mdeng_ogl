#include "PhysicalDevice.h"
#include "../Instance/Instance.h"
#include "../Surface/Surface.h"
#include "../Device/Device.h"
#include <bit>
#include <cassert>
#include "Core/Utils/Binary.hpp"

namespace OpenGL
{
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

    static Render::PhysicalDeviceProperties
    get_physical_device_properties(GladGLContext& loader, bool robust_buffer_access)
    {
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

        Render::PhysicalDeviceLimits limits = Render::PhysicalDeviceLimits{
            .max_image_dimension_1D = get_property<std::uint32_t>(loader, GL_MAX_TEXTURE_SIZE),
            .max_image_dimension_2D = get_property<std::uint32_t>(loader, GL_MAX_TEXTURE_SIZE),
            .max_image_dimension_3D = get_property<std::uint32_t>(loader, GL_MAX_3D_TEXTURE_SIZE),
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
            .max_per_stage_descriptor_uniform_buffers = max_per_stage_descriptor_uniform_buffers,
            .max_per_stage_descriptor_storage_buffers = max_per_stage_descriptor_storage_buffers,
            .max_per_stage_descriptor_sampled_images = max_per_stage_descriptor_textures,
            .max_per_stage_descriptor_storage_images = max_per_stage_descriptor_storage_images,
            .max_per_stage_descriptor_input_attachments = max_per_stage_descriptor_textures,
            .max_per_stage_resources =
                max_per_stage_descriptor_textures + max_per_stage_descriptor_uniform_buffers +
                max_per_stage_descriptor_storage_buffers + max_per_stage_descriptor_storage_images,
            .max_descriptor_set_samplers = std::numeric_limits<std::uint32_t>::max(),
            .max_descriptor_set_uniform_buffers = std::numeric_limits<std::uint32_t>::max(),
            .max_descriptor_set_storage_buffers = std::numeric_limits<std::uint32_t>::max(),
            .max_descriptor_set_sampled_images = std::numeric_limits<std::uint32_t>::max(),
            .max_descriptor_set_storage_images = std::numeric_limits<std::uint32_t>::max(),
            .max_descriptor_set_input_attachments = std::numeric_limits<std::uint32_t>::max(),
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
                get_property<std::uint32_t>(loader, GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS),
            .max_tessellation_evaluation_input_components =
                get_property<std::uint32_t>(loader, GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS),
            .max_tessellation_evaluation_output_components =
                get_property<std::uint32_t>(loader, GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS),
            .max_geometry_shader_invocations =
                get_property<std::uint32_t>(loader, GL_MAX_GEOMETRY_SHADER_INVOCATIONS),
            .max_geometry_input_components =
                get_property<std::uint32_t>(loader, GL_MAX_GEOMETRY_INPUT_COMPONENTS),
            .max_geometry_output_components =
                get_property<std::uint32_t>(loader, GL_MAX_GEOMETRY_OUTPUT_COMPONENTS),
            .max_geometry_output_vertices =
                get_property<std::uint32_t>(loader, GL_MAX_GEOMETRY_OUTPUT_VERTICES),
            .max_geometry_total_output_components =
                get_property<std::uint32_t>(loader, GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS),
            .max_fragment_input_components =
                get_property<std::uint32_t>(loader, GL_MAX_FRAGMENT_INPUT_COMPONENTS),
            .max_fragment_output_attachments =
                get_property<std::uint32_t>(loader, GL_MAX_DRAW_BUFFERS),
            .max_fragment_dual_src_attachments =
                get_property<std::uint32_t>(loader, GL_MAX_DUAL_SOURCE_DRAW_BUFFERS),
            .max_fragment_combined_output_resources =
                get_property<std::uint32_t>(loader, GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES),
            .max_compute_shared_memory_size =
                get_property<std::uint32_t>(loader, GL_MAX_COMPUTE_SHARED_MEMORY_SIZE),
            .max_compute_work_group_count =
                get_property<Render::ComputeGroupSize>(loader, GL_MAX_COMPUTE_WORK_GROUP_COUNT),
            .max_compute_work_group_invocations =
                get_property<std::uint32_t>(loader, GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS),
            .max_compute_work_group_size =
                get_property<Render::ComputeGroupSize>(loader, GL_MAX_COMPUTE_WORK_GROUP_SIZE),
            .sub_pixel_precision_bits = get_property<std::uint32_t>(loader, GL_SUBPIXEL_BITS),
            .max_draw_indexed_index_value =
                std::numeric_limits<std::uint32_t>::max() - 1 /*GL_MAX_ELEMENT_INDEX*/,
            .max_draw_indirect_count = std::numeric_limits<std::uint32_t>::max(),
            .max_sampler_lod_bias = get_property<float>(loader, GL_MAX_TEXTURE_LOD_BIAS),
            .max_sampler_anisotropy =
                ((loader.VERSION_4_6 || loader.ARB_texture_filter_anisotropic) ?
                     get_property<float>(loader, GL_MAX_TEXTURE_MAX_ANISOTROPY) :
                     1.0f),
            .max_viewports = get_property<std::uint32_t>(loader, GL_MAX_VIEWPORTS),
            .max_viewport_dimensions = get_property<Render::Extent2D>(loader, GL_MAX_VIEWPORT_DIMS),
            .viewport_bounds_range = get_property<Render::Range>(loader, GL_VIEWPORT_BOUNDS_RANGE),
            .viewport_sub_pixel_bits =
                get_property<std::uint32_t>(loader, GL_VIEWPORT_SUBPIXEL_BITS),
            .min_memory_map_alignment =
                get_property<std::uint32_t>(loader, GL_MIN_MAP_BUFFER_ALIGNMENT),
            .min_texel_buffer_offset_alignment =
                get_property<std::uint32_t>(loader, GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT),
            .min_uniform_buffer_offset_alignment =
                get_property<std::uint32_t>(loader, GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT),
            .min_storage_buffer_offset_alignment =
                get_property<std::uint32_t>(loader, GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT),
            .min_texel_offset = get_property<std::int32_t>(loader, GL_MIN_PROGRAM_TEXEL_OFFSET),
            .max_texel_offset = get_property<std::uint32_t>(loader, GL_MAX_PROGRAM_TEXEL_OFFSET),
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
            .max_framebuffer_width = get_property<std::uint32_t>(loader, GL_MAX_FRAMEBUFFER_WIDTH),
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
            .max_color_attachments = get_property<std::uint32_t>(loader, GL_MAX_COLOR_ATTACHMENTS),
            .sampled_image_color_sample_counts =
                get_property<Render::SampleCountFlags>(loader, GL_MAX_COLOR_TEXTURE_SAMPLES),
            .sampled_image_integer_sample_counts =
                get_property<Render::SampleCountFlags>(loader, GL_MAX_INTEGER_SAMPLES),
            .sampled_image_depth_sample_counts =
                get_property<Render::SampleCountFlags>(loader, GL_MAX_DEPTH_TEXTURE_SAMPLES),
            .sampled_image_stencil_sample_counts =
                get_property<Render::SampleCountFlags>(loader, GL_MAX_DEPTH_TEXTURE_SAMPLES),
            .storage_image_sample_counts =
                get_property<Render::SampleCountFlags>(loader, GL_MAX_IMAGE_SAMPLES),
            .max_sample_mask_words = get_property<std::uint32_t>(loader, GL_MAX_SAMPLE_MASK_WORDS),
            .max_clip_distances = get_property<std::uint32_t>(loader, GL_MAX_CLIP_DISTANCES),
            .max_cull_distances = get_property<std::uint32_t>(loader, GL_MAX_CULL_DISTANCES),
            .max_combined_clip_and_cull_distances =
                get_property<std::uint32_t>(loader, GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES),
            .discrete_queue_priorities = std::numeric_limits<std::uint32_t>::max(),
            .point_size_range = get_property<Render::Range>(loader, GL_POINT_SIZE_RANGE),
            .line_width_range = get_property<Render::Range>(loader, GL_SMOOTH_LINE_WIDTH_RANGE),
            .point_size_granularity = get_property<float>(loader, GL_POINT_SIZE_GRANULARITY),
            .line_width_granularity = get_property<float>(loader, GL_SMOOTH_LINE_WIDTH_GRANULARITY),
            .optimal_buffer_copy_offset_alignment = 1,
            .optimal_buffer_copy_row_pitch_alignment = 1,
            .non_coherent_atom_size = 1,
            .max_custom_border_color_samplers = std::numeric_limits<std::uint32_t>::max()};

        Render::PhysicalDeviceFeatures features = Render::PhysicalDeviceFeatures{
            .robust_buffer_access = robust_buffer_access,
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
            .depth_bounds = static_cast<bool>(loader.EXT_depth_bounds_test),
            .wide_lines = !(limits.line_width_range.min == limits.line_width_range.max &&
                            limits.line_width_range.min == 1.0f),
            .large_points = !(limits.point_size_range.min == limits.point_size_range.max &&
                              limits.point_size_range.min == 1.0f),
            .alpha_to_one = true,
            .multi_viewport = (limits.max_viewports > 1),
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
            .index_type_uint8 = true};

        return Render::PhysicalDeviceProperties{
            .version = Render::MakeVersion(major, minor),
            .vendor_name = reinterpret_cast<const char*>(loader.GetString(GL_VENDOR)),
            .device_name = reinterpret_cast<const char*>(loader.GetString(GL_RENDERER)),
            .extensions = std::move(extensions),
            .supported_syntax = Render::ShaderSyntaxFlagBits::GLSL,
            .queue_family_properties = {Render::QueueFamilyProperties{
                .specialization = Render::QueueSpecializationFlagBits::TransferSpec |
                                  Render::QueueSpecializationFlagBits::ComputeSpec |
                                  Render::QueueSpecializationFlagBits::GraphicsSpec,
                .queue_count = 1,
                .min_image_transfer_granularity =
                    Render::Extent3D{.width = 1, .height = 1, .depth = 1}}},
            .memory_types =
                std::vector(std::begin(AVAILABLE_MEMORY_TYPES), std::end(AVAILABLE_MEMORY_TYPES)),
            .command_buffer_strategy = Render::CommandBufferStrategy::Immediate,
            .device_type = Render::PhysicalDeviceType::Other,
            .view_origin = Render::ViewOrigin::BottomLeft,
            .clip_space_depth_bounds =
                Render::ClipSpaceDepthBounds{.min = 0.0f,
                                             .max = 1.0f}, //due to glClipControl(GL_ZERO_TO_ONE)
            .limits = limits,
            .features = features};
    }

    PhysicalDevice::PhysicalDevice(Instance* _parent)
        : PhysicalDeviceBase(_parent),
          parent(_parent),
          device(nullptr)
    {
        device = nullptr;

        int glad_ver =
            gladLoadGLContext(&loader, reinterpret_cast<GLADloadfunc>(wglGetProcAddress));
        if(glad_ver == 0)
            throw std::runtime_error("Failed to load GLAD");

        bool debug_messenger_enabled = parent->GetEnabledFeatures().validation_layer ||
                                       parent->GetEnabledFeatures().debug_messenger;

        if(debug_messenger_enabled)
        {
            const auto& info = parent->GetDebugMessengerInfo();

            EnableDebugMessenger(loader);
            ::OpenGL::SetDebugMessenger(loader, info);
        }

        properties =
            get_physical_device_properties(loader,
                                           this->PhysicalDeviceBase::IsRobustContextSupported());
    }

    PhysicalDevice::~PhysicalDevice()
    {}

    const Render::PhysicalDeviceProperties& PhysicalDevice::GetProperties() const noexcept
    {
        return properties;
    }

    bool PhysicalDevice::GetSurfaceSupport(Render::Surface* surface,
                                           std::uint32_t queue_family_index) const noexcept
    {
        Surface* impl_surface = static_cast<Surface*>(surface);
        if(!impl_surface->IsConnected())
            return true;

        return impl_surface->GetConnectedPhysicalDevice() == this;
    }

    Render::SurfaceCapabilities
    PhysicalDevice::GetSurfaceCapablities(Render::Surface* surface) const noexcept
    {
        Surface* impl_surface = static_cast<Surface*>(surface);

        if(impl_surface->IsConnected())
            return impl_surface->GetConnectedCapabilities();

        return this->PhysicalDeviceBase::GetSurfaceCapabilities();
    }

    std::optional<Render::BufferFormatProperties>
    PhysicalDevice::GetBufferFormatProperties(const Render::BufferFormatInfo& info) const
    {
        Render::BufferFormatProperties props = {.features = {}};

        auto opt = DecodeVertexInputTypeSizePair(info.format);

        if(opt)
            props.features = Render::FormatFeatureFlagBits::FormatFeatureVertexBufferBit;

        Render::FormatFeatureFlags image_format_features = {};
        switch(info.format)
        {
            case Render::Format::R8_UNORM:
            case Render::Format::R16_UNORM:
            case Render::Format::R16_SFLOAT:
            case Render::Format::R32_SFLOAT:
            case Render::Format::R8_SINT:
            case Render::Format::R16_SINT:
            case Render::Format::R32_SINT:
            case Render::Format::R8_UINT:
            case Render::Format::R16_UINT:
            case Render::Format::R32_UINT:
            case Render::Format::R8G8_UNORM:
            case Render::Format::R16G16_UNORM:
            case Render::Format::R16G16_SFLOAT:
            case Render::Format::R32G32_SFLOAT:
            case Render::Format::R8G8_SINT:
            case Render::Format::R16G16_SINT:
            case Render::Format::R32G32_SINT:
            case Render::Format::R8G8_UINT:
            case Render::Format::R16G16_UINT:
            case Render::Format::R32G32_UINT:
            case Render::Format::R32G32B32_SFLOAT:
            case Render::Format::R32G32B32_SINT:
            case Render::Format::R32G32B32_UINT:
            case Render::Format::R8G8B8A8_UNORM:
            case Render::Format::R16G16B16A16_UNORM:
            case Render::Format::R16G16B16A16_SFLOAT:
            case Render::Format::R32G32B32A32_SFLOAT:
            case Render::Format::R8G8B8A8_SINT:
            case Render::Format::R16G16B16A16_SINT:
            case Render::Format::R32G32B32A32_SINT:
            case Render::Format::R8G8B8A8_UINT:
            case Render::Format::R16G16B16A16_UINT:
            case Render::Format::R32G32B32A32_UINT:
                image_format_features =
                    Render::FormatFeatureFlagBits::FormatFeatureUniformTexelBufferBit |
                    Render::FormatFeatureFlagBits::FormatFeatureStorageTexelBufferBit;

                if(info.format == Render::Format::R32_SINT ||
                   info.format == Render::Format::R32_UINT)
                    image_format_features |=
                        Render::FormatFeatureFlagBits::FormatFeatureStorageTexelBufferAtomicBit;
                break;
            default:
                image_format_features = {};
                break;
        }

        if(image_format_features != 0)
        {
            auto native_format_opt = FormatToNative(info.format);
            if(native_format_opt)
            {
                GLint format_supported = GL_FALSE;
                loader.GetInternalformativ(GL_TEXTURE_BUFFER,
                                           native_format_opt.value(),
                                           GL_INTERNALFORMAT_SUPPORTED,
                                           1,
                                           &format_supported);
                if(format_supported != GL_FALSE)
                    props.features |= image_format_features;
            }
        }

        if(props.features != 0)
            return props;

        return std::nullopt;
    }

    std::optional<Render::ImageFormatProperties>
    PhysicalDevice::GetImageFormatProperties(const Render::ImageFormatInfo& info) const
    {
        GLenum inner_type =
            DecodeImageType(info.type,
                            info.layered,
                            info.sampled,
                            info.flags & Render::ImageFlagBits::ImageCubeCompatible);
        auto native_format_opt = FormatToNative(info.format);

        if(!native_format_opt)
            return std::nullopt;

        GLenum native_format = native_format_opt.value();

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
            max_mip_levels =
                Render::LOD_CLAMP_NONE; //in OpenGL spec it is a default value -> so do not care...

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
                if(/*samples[i] <= Render::SampleCount::SampleCount_64 &&*/
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
        //FormatFeatureTransferSrcBit; -> decode
        //FormatFeatureTransferDstBit; -> decode

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
                case Render::Format::R32G32B32A32_SFLOAT:
                case Render::Format::R16G16B16A16_SFLOAT:
                case Render::Format::R32G32_SFLOAT:
                case Render::Format::R16G16_SFLOAT:
                case Render::Format::B10G11R11_UFLOAT_PACK32:
                case Render::Format::R32_SFLOAT:
                case Render::Format::R16_SFLOAT:

                case Render::Format::R32G32B32A32_UINT:
                case Render::Format::R16G16B16A16_UINT:
                case Render::Format::A2B10G10R10_UINT_PACK32:
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
                case Render::Format::A2B10G10R10_UNORM_PACK32:
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

            if(DecodeTransferTypeFormatPair(info.format).has_value())
                features |=
                    Render::FormatFeatureTransferSrcBit | Render::FormatFeatureTransferDstBit;
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

    Render::Device* PhysicalDevice::CreateDevice(const Render::DeviceInfo& info)
    {
        if(device != nullptr)
            throw std::runtime_error("Physical device already has created logical device");

        Surface* impl_surface = static_cast<Surface*>(info.surface);
        if(impl_surface->IsConnected())
            throw std::runtime_error("Surface is connected to other device");

        device = new Device(this, info);
        return device;
    }

    Render::Instance* PhysicalDevice::GetParent() const noexcept
    {
        return parent;
    }

    GLADloadfunc PhysicalDevice::GetProcAddressResolver() const noexcept
    {
        return this->PhysicalDeviceBase::GetProcAddressResolver();
    }

    const Render::SurfaceCapabilities
    PhysicalDevice::GetSurfaceCapabilitiesByIndex(std::uint32_t index) const noexcept
    {
        return this->PhysicalDeviceBase::GetSurfaceCapabilitiesByIndex(index);
    }

    void PhysicalDevice::DeleteDeviceNotify() noexcept
    {
        device = nullptr;

        this->PhysicalDeviceBase::MakeCurrent();
    }

    void PhysicalDevice::SetDebugMessenger(const Render::DebugMessengerInfo& info)
    {
        this->PhysicalDeviceBase::MakeCurrent();

        OpenGL::SetDebugMessenger(loader, info);

        if(device)
            device->SetDebugMessenger(info);
    }
};
#include "Core/Utils/Entry.h"

#include <iostream>
#include <vector>
#include <format>
#include "Core/Render/Resolve.h"
#include "Core/Render/Objects/Instance.h"
#include "Core/Render/Objects/PhysicalDevice.h"
#include "Core/Render/Objects/Device.h"
#include "Core/Render/Objects/Surface.h"
#include "Core/Render/Objects/Swapchain.h"
#include <Core/Render/Objects/Semaphore.h>
#include <Core/Render/Objects/Fence.h>
#include "Core/Render/Format.h"
#include "Core/View/View.h"
#include "Core/View/Window.h"
#include "Core/View/WindowEvents.h"
#include "Core/View/WindowSubsystem.h"
#include "Core/View/Display.h"
#include "Core/Utils/DynamicLibrary.h"
#include <cassert>
#include <chrono>
#include <thread>

void PrintInstance(const Render::Resolve* resolve, const Render::Instance* instance)
{
#define PRINT_FEATURE(NAME) \
    std::cout << std::format("\t\t{} = {}\n", #NAME, (features.NAME ? "true" : "false"));

    auto backend = resolve->GetBackend();
    auto features = resolve->GetInstanceFeatures();
    auto surface_backends = resolve->GetAvailableSurfaceBackends();
    auto context_mode = instance->GetContextMode();

    std::cout << "Instance:\n";
    std::cout << std::format("\tBackend: {}\n",
                             (backend == Render::Backend::OpenGL ? "OpenGL" : "Unknown"));
    std::cout << std::format(
        "\tSurface backends: {}\n",
        (surface_backends[0] == Render::SurfaceBackend::Win32 ? "Win32" : "Unknown"));
    std::cout << "\tFeatures:\n";
    PRINT_FEATURE(validation_layer)
    PRINT_FEATURE(debug_messenger)
    std::cout << std::format("\tContext mode: {}\n",
                             (context_mode == Render::ContextMode::Legacy ? "Legacy" : "Strict"));

#undef PRINT_FEATURE
}

void PrintPhysicalDevices(std::span<Render::PhysicalDevice*> physical_devices)
{
    for(std::size_t i = 0; i < physical_devices.size(); i++)
    {
        auto props = physical_devices[i]->GetProperties();

        std::cout << std::format("Physical device #{}\n", i);
        std::cout << std::format("\tVersion: {}.{}\n",
                                 Render::GetMajorVersion(props.version),
                                 Render::GetMinorVersion(props.version));
        std::cout << std::format("\tVendor: {}\n", props.vendor_name);
        std::cout << std::format("\tDevice: {}\n", props.device_name);

        if(props.supported_syntax | Render::ShaderSyntaxFlagBits::GLSL)
            std::cout << "\tShader syntax: GLSL\n";

        std::cout << std::format(
            "\tCommand buffer strategy: {}\n",
            (props.command_buffer_strategy == Render::CommandBufferStrategy::Immediate ?
                 "Immediate" :
                 "Deffered"));

        std::string_view device_type;
        switch(props.device_type)
        {
            case Render::PhysicalDeviceType::CPU:
                device_type = "CPU";
                break;
            case Render::PhysicalDeviceType::DiscreteGPU:
                device_type = "DiscreteGPU";
                break;
            case Render::PhysicalDeviceType::IntegratedGPU:
                device_type = "IntegratedGPU";
                break;
            case Render::PhysicalDeviceType::VirtualGPU:
                device_type = "VirtualGPU";
                break;
            case Render::PhysicalDeviceType::Other:
                device_type = "Other";
                break;
        }

        std::cout << std::format("\tDevice type: {}\n", device_type);
        std::cout << std::format(
            "\tView origin: {}\n",
            (props.view_origin == Render::ViewOrigin::TopLeft ? "TopLeft" : "BottomLeft"));
        std::cout << std::format("\tClip space depth bounds. Min: {}; Max: {}\n",
                                 props.clip_space_depth_bounds.min,
                                 props.clip_space_depth_bounds.max);

        std::cout << "\tMemory types:\n";
        for(std::size_t j = 0; j < props.memory_types.size(); j++)
        {
            auto memory_type = props.memory_types[j];

            std::cout << std::format("\t\tMemory type #{}\n", j);
            std::cout << std::format(
                "\t\t\tHeap flags: {}\n",
                (memory_type.memory_heap_flags & Render::MemoryHeapFlagBits::DeviceLocalHeap ?
                     "DeviceLocalHeap" :
                     "0"));

            std::string type_flags = "";
            for(const auto& type:
                {std::pair{Render::MemoryTypePropertyFlagBits::DeviceLocal, "DeviceLocal"},
                 std::pair{Render::MemoryTypePropertyFlagBits::HostVisible, "HostVisible"},
                 std::pair{Render::MemoryTypePropertyFlagBits::HostCoherent, "HostCoherent"},
                 std::pair{Render::MemoryTypePropertyFlagBits::HostCached, "HostCached"}})
            {
                if(!(memory_type.memory_type_flags & type.first))
                    continue;

                if(type_flags.empty())
                    type_flags = type.second;
                else
                    type_flags += std::format(" | {}", type.second);
            }

            std::cout << std::format("\t\t\tType flags: {}\n",
                                     (!type_flags.empty() ? type_flags : "0"));
        }

        std::cout << "\tExtensions:\n";
        for(std::size_t j = 0; j < props.extensions.size(); j++)
        {
            std::cout << std::format("\t\t#{}: {}\n", j, props.extensions[j]);
        }

        std::cout << "\tQueues:\n";
        for(std::size_t j = 0; j < props.queue_family_properties.size(); j++)
        {
            const auto& queue_family = props.queue_family_properties[j];

            std::cout << std::format("\t\tQueue family #{}\n", j);
            std::cout << std::format("\t\t\tQueue count: {}\n", queue_family.queue_count);
            std::cout << std::format("\t\t\tMin image transfer granularity: {}.{}.{}\n",
                                     queue_family.min_image_transfer_granularity.width,
                                     queue_family.min_image_transfer_granularity.height,
                                     queue_family.min_image_transfer_granularity.depth);

            std::string spec_flags = "";
            for(const auto& type:
                {std::pair{Render::QueueSpecializationFlagBits::TransferSpec, "TransferSpec"},
                 std::pair{Render::QueueSpecializationFlagBits::GraphicsSpec, "GraphicsSpec"},
                 std::pair{Render::QueueSpecializationFlagBits::ComputeSpec, "ComputeSpec"},
                 std::pair{Render::QueueSpecializationFlagBits::UnknownImplementationSpec,
                           "UnknownImplementationSpec"}})
            {
                if(!(queue_family.specialization & type.first))
                    continue;

                if(spec_flags.empty())
                    spec_flags = type.second;
                else
                    spec_flags += std::format(" | {}", type.second);
            }

            std::cout << std::format("\t\t\tSpecialization flags: {}\n",
                                     (!spec_flags.empty() ? spec_flags : "0"));
        }

        std::cout << "\tFeatures:\n";
#define PRINT_FEATURE(NAME) \
    std::cout << std::format("\t\t{} = {}\n", #NAME, (props.features.NAME ? "true" : "false"));

        PRINT_FEATURE(robust_buffer_access);
        PRINT_FEATURE(full_draw_index_uint32);
        PRINT_FEATURE(image_cube_array);
        PRINT_FEATURE(independent_blend);
        PRINT_FEATURE(geometry_shader);
        PRINT_FEATURE(tessellation_shader);
        PRINT_FEATURE(sample_rate_shading);
        PRINT_FEATURE(dual_src_blend);
        PRINT_FEATURE(logic_op);
        PRINT_FEATURE(multi_draw_indirect);
        PRINT_FEATURE(draw_indirect_first_instance);
        PRINT_FEATURE(depth_clamp);
        PRINT_FEATURE(depth_bias_clamp);
        PRINT_FEATURE(fill_mode_non_solid);
        PRINT_FEATURE(depth_bounds);
        PRINT_FEATURE(wide_lines);
        PRINT_FEATURE(large_points);
        PRINT_FEATURE(alpha_to_one);
        PRINT_FEATURE(multi_viewport);
        PRINT_FEATURE(sampler_anisotropy);
        PRINT_FEATURE(vertex_pipeline_stores_and_atomics);
        PRINT_FEATURE(fragment_stores_and_atomics);
        PRINT_FEATURE(shader_tessellation_and_geometry_point_size);
        PRINT_FEATURE(shader_image_gather_extended);
        PRINT_FEATURE(shader_storage_image_multisample);
        PRINT_FEATURE(shader_storage_image_read_without_format);
        PRINT_FEATURE(shader_storage_image_write_without_format);
        PRINT_FEATURE(shader_uniform_buffer_array_dynamic_indexing);
        PRINT_FEATURE(shader_sampled_image_array_dynamic_indexing);
        PRINT_FEATURE(shader_storage_buffer_array_dynamic_indexing);
        PRINT_FEATURE(shader_storage_image_array_dynamic_indexing);
        PRINT_FEATURE(shader_clip_distance);
        PRINT_FEATURE(shader_cull_distance);
        PRINT_FEATURE(shader_float64);
        PRINT_FEATURE(shader_int64);
        PRINT_FEATURE(shader_int16);
        PRINT_FEATURE(shader_resource_min_lod);
        PRINT_FEATURE(variable_multisample_rate);
        PRINT_FEATURE(sampler_mirror_clamp_to_edge);
        PRINT_FEATURE(custom_border_colors);
        PRINT_FEATURE(custom_border_color_without_format);
        PRINT_FEATURE(index_type_uint8);

        std::cout << "\tLimits:\n";
#define PRINT_LIMIT_U32(NAME) std::cout << std::format("\t\t{} = {}\n", #NAME, props.limits.NAME);
#define PRINT_LIMIT_U64(NAME) std::cout << std::format("\t\t{} = {}\n", #NAME, props.limits.NAME);
#define PRINT_LIMIT_COMPUTE_GROUP_SIZE(NAME) \
    std::cout << std::format("\t\t{} = {}.{}.{}\n", \
                             #NAME, \
                             props.limits.NAME.x, \
                             props.limits.NAME.y, \
                             props.limits.NAME.z);
#define PRINT_LIMIT_F32(NAME) std::cout << std::format("\t\t{} = {}\n", #NAME, props.limits.NAME);
#define PRINT_LIMIT_EXTENT2D(NAME) \
    std::cout << std::format("\t\t{} = {}\n", \
                             #NAME, \
                             props.limits.NAME.width, \
                             props.limits.NAME.height);
#define PRINT_LIMIT_RANGE(NAME) \
    std::cout << std::format("\t\t{} = {}\n", #NAME, props.limits.NAME.min, props.limits.NAME.max);
#define PRINT_LIMIT_SIZE(NAME) std::cout << std::format("\t\t{} = {}\n", #NAME, props.limits.NAME);
#define PRINT_LIMIT_I32(NAME) std::cout << std::format("\t\t{} = {}\n", #NAME, props.limits.NAME);
#define PRINT_LIMIT_SAMPLES(NAME) \
    { \
        auto samples = props.limits.NAME; \
        std::string value = ""; \
        for(std::underlying_type_t<Render::SampleCount> i = 1; \
            i <= std::numeric_limits<decltype(i)>::digits; \
            i <<= 1) \
        { \
            if(samples & i) \
            { \
                if(value.empty()) \
                    value = std::format("{}", i); \
                else \
                    value += std::format(" | {}", i); \
            } \
        } \
        std::cout << std::format("\t\t{} = {}\n", #NAME, value); \
    }

        PRINT_LIMIT_U32(max_image_dimension_1D);
        PRINT_LIMIT_U32(max_image_dimension_2D);
        PRINT_LIMIT_U32(max_image_dimension_3D);
        PRINT_LIMIT_U32(max_image_dimension_cube);
        PRINT_LIMIT_U32(max_image_array_layers);
        PRINT_LIMIT_U32(max_texel_buffer_elements);
        PRINT_LIMIT_U32(max_uniform_buffer_range);
        PRINT_LIMIT_U64(max_storage_buffer_range);
        PRINT_LIMIT_U32(max_uniform_size);
        PRINT_LIMIT_U32(max_sampler_allocation_count);
        PRINT_LIMIT_U32(max_bound_descriptor_sets);
        PRINT_LIMIT_U32(max_per_stage_descriptor_samplers);
        PRINT_LIMIT_U32(max_per_stage_descriptor_uniform_buffers);
        PRINT_LIMIT_U32(max_per_stage_descriptor_storage_buffers);
        PRINT_LIMIT_U32(max_per_stage_descriptor_sampled_images);
        PRINT_LIMIT_U32(max_per_stage_descriptor_storage_images);
        PRINT_LIMIT_U32(max_per_stage_descriptor_input_attachments);
        PRINT_LIMIT_U32(max_per_stage_resources);
        PRINT_LIMIT_U32(max_descriptor_set_samplers);
        PRINT_LIMIT_U32(max_descriptor_set_uniform_buffers);
        PRINT_LIMIT_U32(max_descriptor_set_storage_buffers);
        PRINT_LIMIT_U32(max_descriptor_set_sampled_images);
        PRINT_LIMIT_U32(max_descriptor_set_storage_images);
        PRINT_LIMIT_U32(max_descriptor_set_input_attachments);
        PRINT_LIMIT_U32(max_vertex_input_attributes);
        PRINT_LIMIT_U32(max_vertex_input_bindings);
        PRINT_LIMIT_U32(max_vertex_input_attribute_offset);
        PRINT_LIMIT_U32(max_vertex_input_binding_stride);
        PRINT_LIMIT_U32(max_vertex_output_components);
        PRINT_LIMIT_U32(max_tessellation_generation_level);
        PRINT_LIMIT_U32(max_tessellation_patch_size);
        PRINT_LIMIT_U32(max_tessellation_control_per_vertex_input_components);
        PRINT_LIMIT_U32(max_tessellation_control_per_vertex_output_components);
        PRINT_LIMIT_U32(max_tessellation_control_per_patch_output_components);
        PRINT_LIMIT_U32(max_tessellation_control_total_output_components);
        PRINT_LIMIT_U32(max_tessellation_evaluation_input_components);
        PRINT_LIMIT_U32(max_tessellation_evaluation_output_components);
        PRINT_LIMIT_U32(max_geometry_shader_invocations);
        PRINT_LIMIT_U32(max_geometry_input_components);
        PRINT_LIMIT_U32(max_geometry_output_components);
        PRINT_LIMIT_U32(max_geometry_output_vertices);
        PRINT_LIMIT_U32(max_geometry_total_output_components);
        PRINT_LIMIT_U32(max_fragment_input_components);
        PRINT_LIMIT_U32(max_fragment_output_attachments);
        PRINT_LIMIT_U32(max_fragment_dual_src_attachments);
        PRINT_LIMIT_U32(max_fragment_combined_output_resources);
        PRINT_LIMIT_U32(max_compute_shared_memory_size);
        PRINT_LIMIT_COMPUTE_GROUP_SIZE(max_compute_work_group_count);
        PRINT_LIMIT_U32(max_compute_work_group_invocations);
        PRINT_LIMIT_COMPUTE_GROUP_SIZE(max_compute_work_group_size);
        PRINT_LIMIT_U32(sub_pixel_precision_bits);
        PRINT_LIMIT_U32(max_draw_indexed_index_value);
        PRINT_LIMIT_U32(max_draw_indirect_count);
        PRINT_LIMIT_F32(max_sampler_lod_bias);
        PRINT_LIMIT_F32(max_sampler_anisotropy);
        PRINT_LIMIT_U32(max_viewports);
        PRINT_LIMIT_EXTENT2D(max_viewport_dimensions);
        PRINT_LIMIT_RANGE(viewport_bounds_range);
        PRINT_LIMIT_U32(viewport_sub_pixel_bits);
        PRINT_LIMIT_SIZE(min_memory_map_alignment);
        PRINT_LIMIT_U64(min_texel_buffer_offset_alignment);
        PRINT_LIMIT_U64(min_uniform_buffer_offset_alignment);
        PRINT_LIMIT_U64(min_storage_buffer_offset_alignment);
        PRINT_LIMIT_I32(min_texel_offset);
        PRINT_LIMIT_U32(max_texel_offset);
        PRINT_LIMIT_I32(min_texel_gather_offset);
        PRINT_LIMIT_U32(max_texel_gather_offset);
        PRINT_LIMIT_F32(min_interpolation_offset);
        PRINT_LIMIT_F32(max_interpolation_offset);
        PRINT_LIMIT_U32(sub_pixel_interpolation_offset_bits);
        PRINT_LIMIT_U32(max_framebuffer_width);
        PRINT_LIMIT_U32(max_framebuffer_height);
        PRINT_LIMIT_U32(max_framebuffer_layers);
        PRINT_LIMIT_SAMPLES(framebuffer_color_sample_counts);
        PRINT_LIMIT_SAMPLES(framebuffer_depth_sample_counts);
        PRINT_LIMIT_SAMPLES(framebuffer_stencil_sample_counts);
        PRINT_LIMIT_SAMPLES(framebuffer_no_attachments_sample_counts);
        PRINT_LIMIT_U32(max_color_attachments);
        PRINT_LIMIT_SAMPLES(sampled_image_color_sample_counts);
        PRINT_LIMIT_SAMPLES(sampled_image_integer_sample_counts);
        PRINT_LIMIT_SAMPLES(sampled_image_depth_sample_counts);
        PRINT_LIMIT_SAMPLES(sampled_image_stencil_sample_counts);
        PRINT_LIMIT_SAMPLES(storage_image_sample_counts);
        PRINT_LIMIT_U32(max_sample_mask_words);
        PRINT_LIMIT_U32(max_clip_distances);
        PRINT_LIMIT_U32(max_cull_distances);
        PRINT_LIMIT_U32(max_combined_clip_and_cull_distances);
        PRINT_LIMIT_U32(discrete_queue_priorities);
        PRINT_LIMIT_RANGE(point_size_range);
        PRINT_LIMIT_RANGE(line_width_range);
        PRINT_LIMIT_F32(point_size_granularity);
        PRINT_LIMIT_F32(line_width_granularity);
        PRINT_LIMIT_U64(optimal_buffer_copy_offset_alignment);
        PRINT_LIMIT_U64(optimal_buffer_copy_row_pitch_alignment);
        PRINT_LIMIT_U64(non_coherent_atom_size);
        PRINT_LIMIT_U32(max_custom_border_color_samplers);
    }

#undef PRINT_FEATURE
}

void PrintSurfaceCaps(const Render::SurfaceCapabilities& surface_caps,
                      const Render::LegacyPhysicalDeviceFeatures* legacy_features)
{
    std::cout << "Surface:\n";
    std::cout << std::format("\tMin image count: {}\n", surface_caps.min_image_count);
    std::cout << std::format("\tMax image count: {}\n", surface_caps.max_image_count);
    std::cout << "\tSurface formats:\n";
    for(std::size_t i = 0; i < surface_caps.supported_formats.size(); i++)
    {
        std::cout << std::format("\t\t#{}: {}\n",
                                 i,
                                 Render::FormatToString(surface_caps.supported_formats[i]));
    }

    std::string present_modes = "";
    for(const auto& mode: {std::pair{Render::PresentModeFlagBits::FIFO, "FIFO"},
                           std::pair{Render::PresentModeFlagBits::RelaxedFIFO, "RelaxedFIFO"},
                           std::pair{Render::PresentModeFlagBits::Immediate, "Immediate"},
                           std::pair{Render::PresentModeFlagBits::Mailbox, "Mailbox"}})
    {
        if(!(surface_caps.supported_present_modes & mode.first))
            continue;

        if(present_modes.empty())
            present_modes = mode.second;
        else
            present_modes += std::format(" | {}", mode.second);
    }

    std::cout << std::format("\tpresent modes: {}\n", present_modes);
    if(legacy_features)
    {
#define PRINT_FEATURE(NAME) \
    std::cout << std::format("\t\t{} = {}\n", #NAME, (legacy_features->NAME ? "true" : "false"));

        std::cout << "\tFeatures:\n";
        PRINT_FEATURE(robust_buffer_access)

#undef PRINT_FEATURE
    }
}

std::string ConcatMouseButtons(Core::MouseButtonFlags buttons)
{
    constexpr static std::pair<Core::MouseButtonFlagBits, std::string_view> BUTTON_NAMES[] = {
        {Core::MouseButtonFlagBits::LeftButton, "Left"},
        {Core::MouseButtonFlagBits::MiddleButton, "Middle"},
        {Core::MouseButtonFlagBits::RightButton, "Right"},
        {Core::MouseButtonFlagBits::X1Button, "X1"},
        {Core::MouseButtonFlagBits::X2Button, "X2"},
    };

    std::string out;
    for(const auto& [id, name]: BUTTON_NAMES)
    {
        if(buttons & id)
        {
            if(!out.empty())
                out += " | ";

            out += name;
        }
    }

    return out;
}

std::string ConcatModifiers(Core::ModifierKeyFlags mods)
{
    constexpr static std::pair<Core::ModifierKeyFlagBits, std::string_view> MOD_NAMES[] = {
        {Core::ModifierKeyFlagBits::LeftShift, "LeftShift"},
        {Core::ModifierKeyFlagBits::RightShift, "RightShift"},
        {Core::ModifierKeyFlagBits::LeftControl, "LeftControl"},
        {Core::ModifierKeyFlagBits::RightControl, "RightControl"},
        {Core::ModifierKeyFlagBits::LeftAlt, "LeftAlt"},
        {Core::ModifierKeyFlagBits::RightAlt, "RightAlt"},
        {Core::ModifierKeyFlagBits::LeftGUI, "LeftGUI"},
        {Core::ModifierKeyFlagBits::RightGUI, "RightGUI"},
    };

    std::string out;
    for(const auto& [id, name]: MOD_NAMES)
    {
        if(mods & id)
        {
            if(!out.empty())
                out += " | ";

            out += name;
        }
    }

    return out;
}

int EntryPoint(std::span<const std::string_view> arguments)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    /*std::setlocale(LC_ALL, "ru");
    auto vk = VK_SPACE;
    auto scancode = MapVirtualKeyExW(vk, MAPVK_VK_TO_VSC_EX, GetKeyboardLayout(0));
    BYTE state[256] = {};
    GetKeyboardState(state);
    std::wstring wstr(64, L'\0');
    auto res = ToUnicodeEx(vk, scancode, state, wstr.data(), 64, 0, GetKeyboardLayout(0));
    if(res < 0)
        std::cerr << "DEAD\n";
    else if(res == 0)
        std::cerr << "None\n";
    else
        std::cerr << std::format("{}\n", Core::System::WideToUTF8(wstr));
    LPARAM l_param = 0;
    l_param |= (scancode & 0xff) << 16;
    if((scancode >> 8) & 0xE0 || (scancode >> 8) & 0xE1)
        l_param |= 0b1 << 23;

    wstr.resize(64, L'\0');
    int count = GetKeyNameTextW(l_param, wstr.data(), 64);
    if(count == 0)
        std::cerr << "ERROR\n";
    else
        std::cerr << std::format("{}\n", Core::System::WideToUTF8(wstr));*/

#endif
    try
    {
        auto win_sys_types = Core::GetAvailableWindowSubsystemTypes();
        assert(std::find(win_sys_types.begin(),
                         win_sys_types.end(),
                         Core::WindowSubsystemType::Win32) != win_sys_types.end());

        Core::WindowSubsystemInfo win_sys_info = {.type = Core::WindowSubsystemType::Win32};
        std::unique_ptr<Core::WindowSubsystem> win_sys(Core::CreateWindowSubsystem(win_sys_info));

        Core::WindowInfo win_info = {.resolution =
                                         Core::WindowResolution{.width = 800, .height = 600},
                                     .state = Core::WindowState::Windowed,
                                     .title = "test"};
        std::unique_ptr<Core::Window> window(win_sys->CreateWindow(win_info));

        bool is_run = true;
        window->Connect<Core::WindowClosedEvent>(
            [&is_run](const Core::WindowClosedEvent& event)
            {
                std::cout << std::format(
                    "WindowCloseEvent:\n"
                    "\tTimestamp: {}\n",
                    event.timestamp_ms);

                is_run = false;

                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::WindowDisplayChangedEvent>(
            [window = window.get()](const Core::WindowDisplayChangedEvent& event)
            {
                Core::Display* display = window->GetDisplay();
                auto video_mode = display->GetCurrentVideoMode();

                /*std::cout << std::format(
                    "WindowDisplayChangedEvent:\n"
                    "\tTimestamp: {}\n"
                    "\tDisplay:\n"
                    "\t\tName: {}\n"
                    "\t\tScale factor: {}\n"
                    "\t\tDispaly Scale factor: {}\n"
                    "\t\tVideo mode:\n"
                    "\t\t\tWidth: {}\n"
                    "\t\t\tHeight: {}\n"
                    "\t\t\tBPP: {}\n"
                    "\t\t\tFrequency: {}\n",
                    event.timestamp_ms,
                    display->GetName(),
                    display->GetScaleFactor(),
                    display->GetDisplayScaleFactor(),
                    video_mode.width,
                    video_mode.height,
                    video_mode.bits_per_pixel,
                    video_mode.refresh_rate);*/

                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::WindowMovedEvent>(
            [](const Core::WindowMovedEvent& event)
            {
                /*std::cout << std::format(
                    "WindowMovedEvent:\n"
                    "\tTimestamp: {}\n"
                    "\tPosition:\n"
                    "\t\tX: {}\n"
                    "\t\tY: {}\n",
                    event.timestamp_ms,
                    event.position.x,
                    event.position.y);*/

                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::WindowResizedEvent>(
            [](const Core::WindowResizedEvent& event)
            {
                /*std::cout << std::format(
                    "WindowResizedEvent:\n"
                    "\tTimestamp: {}\n"
                    "\tResolution:\n"
                    "\t\tWidth: {}\n"
                    "\t\tHeight: {}\n"
                    "\tScaled Resolution:\n"
                    "\t\tWidth: {}\n"
                    "\t\tHeight: {}\n",
                    event.timestamp_ms,
                    event.resolution.width,
                    event.resolution.height,
                    event.scaled_resolution.width,
                    event.scaled_resolution.height);*/

                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::WindowMinimizedEvent>(
            [](const Core::WindowMinimizedEvent& event)
            {
                /*std::cout << std::format(
                    "WindowMinimizedEvent:\n"
                    "\tTimestamp: {}\n"
                    "\tResolution:\n"
                    "\t\tWidth: {}\n"
                    "\t\tHeight: {}\n"
                    "\tScaled Resolution:\n"
                    "\t\tWidth: {}\n"
                    "\t\tHeight: {}\n",
                    event.timestamp_ms,
                    event.resolution.width,
                    event.resolution.height,
                    event.scaled_resolution.width,
                    event.scaled_resolution.height);*/

                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::WindowMaximizedEvent>(
            [](const Core::WindowMaximizedEvent& event)
            {
                /*std::cout << std::format(
                    "WindowMaximizedEvent:\n"
                    "\tTimestamp: {}\n"
                    "\tResolution:\n"
                    "\t\tWidth: {}\n"
                    "\t\tHeight: {}\n"
                    "\tScaled Resolution:\n"
                    "\t\tWidth: {}\n"
                    "\t\tHeight: {}\n",
                    event.timestamp_ms,
                    event.resolution.width,
                    event.resolution.height,
                    event.scaled_resolution.width,
                    event.scaled_resolution.height);*/

                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::WindowHiddenEvent>(
            [](const Core::WindowHiddenEvent& event)
            {
                /*std::cout << std::format(
                    "WindowHiddenEvent:\n"
                    "\tTimestamp: {}\n",
                    event.timestamp_ms);*/

                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::WindowShownEvent>(
            [](const Core::WindowShownEvent& event)
            {
                /*std::cout << std::format(
                    "WindowShownEvent:\n"
                    "\tTimestamp: {}\n",
                    event.timestamp_ms);*/

                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::WindowCursorFocusGainEvent>(
            [](const Core::WindowCursorFocusGainEvent& event)
            {
                /*std::cout << std::format(
                    "WindowCursorFocusGainEvent:\n"
                    "\tTimestamp: {}\n"
                    "\tButtons: {}\n"
                    "\tCursor position:\n"
                    "\t\tX: {}\n"
                    "\t\tY: {}\n",
                    event.timestamp_ms,
                    ConcatMouseButtons(event.buttons),
                    event.cursor_position.x,
                    event.cursor_position.y);
                */
                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::WindowCursorFocusLeaveEvent>(
            [](const Core::WindowCursorFocusLeaveEvent& event)
            {
                /*std::cout << std::format(
                    "WindowCursorFocusLeaveEvent:\n"
                    "\tTimestamp: {}\n",
                    event.timestamp_ms);
                */
                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::WindowKeyboardFocusGainEvent>(
            [](const Core::WindowKeyboardFocusGainEvent& event)
            {
                /*std::cout << std::format(
                    "WindowKeyboardFocusGainEvent:\n"
                    "\tTimestamp: {}\n",
                    event.timestamp_ms);*/

                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::WindowKeyboardFocusLeaveEvent>(
            [](const Core::WindowKeyboardFocusLeaveEvent& event)
            {
                /*std::cout << std::format(
                    "WindowKeyboardFocusLeaveEvent:\n"
                    "\tTimestamp: {}\n",
                    event.timestamp_ms);*/

                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::MouseButtonPressedEvent>(
            [window = window.get()](const Core::MouseButtonPressedEvent& event)
            {
                /*std::cout << std::format(
                    "MouseButtonPressedEvent:\n"
                    "\tTimestamp: {}\n"
                    "\tButton: {}\n"
                    "\tClicks: {}\n"
                    "\tCursor position:\n"
                    "\t\tX: {}\n"
                    "\t\tY: {}\n",
                    event.timestamp_ms,
                    ConcatMouseButtons(event.button),
                    event.clicks,
                    event.cursor_position.x,
                    event.cursor_position.y);*/

                if(event.button == Core::MouseButtonFlagBits::LeftButton)
                    window->GetParent()->SetKeyboardAccessState(
                        Core::KeyboardAccessState::Exclusive);
                else if(event.button == Core::MouseButtonFlagBits::RightButton)
                    window->GetParent()->SetKeyboardAccessState(Core::KeyboardAccessState::Shared);

                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::MouseButtonReleasedEvent>(
            [](const Core::MouseButtonReleasedEvent& event)
            {
                /*std::cout << std::format(
                    "MouseButtonReleasedEvent:\n"
                    "\tTimestamp: {}\n"
                    "\tButton: {}\n"
                    "\tCursor position:\n"
                    "\t\tX: {}\n"
                    "\t\tY: {}\n",
                    event.timestamp_ms,
                    ConcatMouseButtons(event.button),
                    event.cursor_position.x,
                    event.cursor_position.y);*/

                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::MouseCursorMoveEvent>(
            [](const Core::MouseCursorMoveEvent& event)
            {
                /*std::cout << std::format(
                    "MouseCursorMoveEvent:\n"
                    "\tTimestamp: {}\n"
                    "\tButtons: {}\n"
                    "\tCursor position:\n"
                    "\t\tX: {}\n"
                    "\t\tY: {}\n",
                    event.timestamp_ms,
                    ConcatMouseButtons(event.buttons),
                    event.cursor_position.x,
                    event.cursor_position.y);
                */
                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::MouseWheelEvent>(
            [window = window.get()](const Core::MouseWheelEvent& event)
            {
                /*std::cout << std::format(
                    "MouseWheelEvent:\n"
                    "\tTimestamp: {}\n"
                    "\tButtons: {}\n"
                    "\tCursor position:\n"
                    "\t\tX: {}\n"
                    "\t\tY: {}\n"
                    "\tXScroll: {}\n"
                    "\tYScroll: {}\n",
                    event.timestamp_ms,
                    ConcatMouseButtons(event.buttons),
                    event.cursor_position.x,
                    event.cursor_position.y,
                    event.x_scroll,
                    event.y_scroll);*/

                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::KeyboardCharacterPressedEvent>(
            [](const Core::KeyboardCharacterPressedEvent& event)
            {
                auto res = Core::System::UTF32ToUTF8(event.utf32_char);
                std::string_view str{res.data, res.length};

                std::cout << std::format(
                    "KeyboardCharacterPressedEvent:\n"
                    "\tTimestamp: {}\n"
                    "\tCModifiers: {}\n"
                    "\tUTF32: {:#x}\n"
                    "\tUTF8: {:#x} {:#x} {:#x} {:#x}\n"
                    "\tChar: {}\n",
                    event.timestamp_ms,
                    ConcatModifiers(event.modifiers),
                    static_cast<std::uint32_t>(event.utf32_char),
                    res.data[0],
                    res.data[1],
                    res.data[2],
                    res.data[3],
                    str);

                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::KeyboardKeyPressedEvent>(
            [window = window.get()](const Core::KeyboardKeyPressedEvent& event)
            {
                /*std::cout << std::format(
                    "KeyboardKeyPressedEvent:\n"
                    "\tTimestamp: {}\n"
                    "\tScancode: {}\n"
                    "\tModifiers: {}\n"
                    "\tName: {}\n",
                    event.timestamp_ms,
                    event.scancode,
                    ConcatModifiers(event.modifiers),
                    Core::KeyboardKeyToString(event.key));*/

                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        window->Connect<Core::KeyboardKeyReleasedEvent>(
            [](const Core::KeyboardKeyReleasedEvent& event)
            {
                /*std::cout << std::format(
                    "KeyboardKeyReleasedEvent:\n"
                    "\tTimestamp: {}\n"
                    "\tScancode: {}\n"
                    "\tModifiers: {}\n"
                    "\tName: {}\n",
                    event.timestamp_ms,
                    event.scancode,
                    ConcatModifiers(event.modifiers),
                    Core::KeyboardKeyToString(event.key));*/

                return Core::EventHandlerResult::None;
            },
            nullptr,
            Core::EventHandlerState::Enabled);

        Core::Display* display = window->GetDisplay();
        std::cerr << std::format("Display: {}\n", display->GetName());
        std::cerr << std::format("Display scale factor: {}\n", display->GetDisplayScaleFactor());
        std::cerr << std::format("Scale factor: {}\n", display->GetScaleFactor());

        auto video_mode = display->GetCurrentVideoMode();
        std::cerr << std::format("Display video mode: width = {}, height = {}, bpp = {}, hz = {}\n",
                                 video_mode.width,
                                 video_mode.height,
                                 video_mode.bits_per_pixel,
                                 video_mode.refresh_rate);

        auto video_modes = display->GetVideoModes();
        for(std::size_t i = 0; i < video_modes.size(); i++)
        {
            std::cerr << std::format(
                "Display video mode #{}: width = {}, height = {}, bpp = {}, hz = {}\n",
                i,
                video_modes[i].width,
                video_modes[i].height,
                video_modes[i].bits_per_pixel,
                video_modes[i].refresh_rate);
        }

        auto title = window->GetTitle();
        window->SetTitle("amogus");

        win_sys->SetCursorState(Core::CursorState::Disbaled);
        win_sys->GetCursorState();
        win_sys->SetCursorState(Core::CursorState::Enabled);

        auto mouse_pos = window->GetMouseCursorPosition();
        window->SetMouseCursorPosition(Core::WindowPosition{.x = 0, .y = 0});

        auto reso = window->GetResolution();
        auto scaled_reso = window->GetScaledResolution();

        window->Resize(Core::WindowResolution{.width = 1440, .height = 900});
        auto after_reso = window->GetResolution();

        //window->GetDisplay()->SetVideoMode(59);
        reso = window->GetResolution();

        window->SetVisibility(Core::WindowVisibility::Hidden);
        window->SetVisibility(Core::WindowVisibility::Shown);

        auto pre_reso = window->GetResolution();
        window->SetState(Core::WindowState::FullScreen);
        auto full_reso = window->GetResolution();
        window->SetState(Core::WindowState::Windowed);
        auto win_reso = window->GetResolution();

        auto window_resolution = window->GetResolution();
        Render::Extent2D swapchain_extent = {
            .width = static_cast<std::uint32_t>(window_resolution.width),
            .height = static_cast<std::uint32_t>(window_resolution.height)};

        Core::DynamicLibrary lib;
        auto lib_path = Core::System::GetExecutablePath().parent_path() /
                        Core::System::DecorateDynamicLibraryName("OpenGLRenderBackend");
        if(auto err = lib.Open(lib_path); err)
            std::rethrow_exception(err);

        Render::PFN_RenderResolve render_resolve = reinterpret_cast<Render::PFN_RenderResolve>(
            lib.GetProcAddress(Render::RENDER_RESOLVE_FUNCTION_NAME));

        if(!render_resolve)
            throw std::runtime_error(std::format("No '{}' in : {}",
                                                 Render::RENDER_RESOLVE_FUNCTION_NAME,
                                                 lib_path.string()));

        std::unique_ptr<Render::Resolve> resolve(render_resolve());
        resolve->Init();

        auto backend = resolve->GetBackend();
        auto features = resolve->GetInstanceFeatures();
        features.debug_messenger = false;

        const Render::InstanceInfo instance_info = {.application_name = "app",
                                                    .application_version = 1,
                                                    .engine_name = "engine",
                                                    .engine_version = 1,
                                                    .enabled_features = features,
                                                    .surface_backend =
                                                        Render::SurfaceBackend::Win32,
                                                    .debug_messenger_info = {}};

        std::unique_ptr<Render::Instance> instance(resolve->CreateInstance(instance_info));

#ifdef _WIN32
        const auto sur_info = std::get<Render::Win32SurfaceInfo>(window->GetWindowSurfaceInfo());
#endif

        std::unique_ptr<Render::Surface> surface(instance->CreateSurface(sur_info));

        std::unique_ptr<Render::Device> device;

        PrintInstance(resolve.get(), instance.get());

        std::uint32_t present_queue_family = 0;

        if(instance->GetContextMode() == Render::ContextMode::Legacy)
        {
            auto legacy_caps = surface->GetLegacySurfaceCapablities();
            auto legacy_features = surface->GetLegacyPhysicalDeviceFeatures();

            PrintSurfaceCaps(legacy_caps, &legacy_features);

            if(legacy_caps.extent_mode == Render::SurfaceExtentMode::Undefined)
            {
                if(!(swapchain_extent.width >= legacy_caps.min_extent.width &&
                     swapchain_extent.height >= legacy_caps.min_extent.height &&
                     swapchain_extent.width <= legacy_caps.max_extent.width &&
                     swapchain_extent.height <= legacy_caps.max_extent.height))
                {
                    throw std::runtime_error(
                        std::format("Cannot create swapchain with extent: {}.{}",
                                    swapchain_extent.width,
                                    swapchain_extent.height));
                }
            }

            device.reset(instance->CreateLegacyDevice(Render::LegacyDeviceInfo{
                .enabled_features =
                    Render::LegacyPhysicalDeviceFeatures{.robust_buffer_access = false},
                .surface = surface.get(),
                .swapchain_info = Render::SwapchainInfo{
                    .min_image_count = legacy_caps.min_image_count,
                    .format = legacy_caps.supported_formats[0],
                    .present_mode = Render::PresentModeFlagBits::FIFO,
                    .extent = (legacy_caps.extent_mode == Render::SurfaceExtentMode::Undefined ?
                                   swapchain_extent :
                                   legacy_caps.current_extent)}}));

            Render::PhysicalDevice* ph_dev = device->GetParent();
            PrintPhysicalDevices(std::span{&ph_dev, 1});
        }
        else
        {
            auto physical_devices = instance->GetPhysicalDevices();

            Render::PhysicalDevice* selected_dev = nullptr;
            for(std::size_t i = 0; i < physical_devices.size(); i++)
            {
                auto props = physical_devices[i]->GetProperties();

                PrintPhysicalDevices(physical_devices);

                std::int64_t present_family_index = -1;
                for(std::size_t j = 0; j < props.queue_family_properties.size(); j++)
                {
                    if(physical_devices[i]->GetSurfaceSupport(surface.get(), j) &&
                       props.queue_family_properties[j].specialization &
                           Render::QueueSpecializationFlagBits::GraphicsSpec)
                    {
                        present_family_index = j;
                        break;
                    }
                }

                if(present_family_index == -1)
                    continue;

                present_queue_family = present_family_index;
                selected_dev = physical_devices[i];
            }

            if(!selected_dev)
                throw std::runtime_error("No compatible physical device found");

            auto surface_caps = selected_dev->GetSurfaceCapablities(surface.get());
            PrintSurfaceCaps(surface_caps, nullptr);

            std::array<float, 1> queue_priorities = {1.0f};

            const Render::DeviceInfo device_info = {
                .queue_family_infos = {Render::QueueFamilyInfo{.index = present_queue_family,
                                                               .queue_count = 1,
                                                               .queue_priorities =
                                                                   queue_priorities.data()}},
                .enabled_features = Render::PhysicalDeviceFeatures{},
                .surface = surface.get(),
                .swapchain_info =
                    Render::SwapchainInfo{.min_image_count = surface_caps.min_image_count,
                                          .format = surface_caps.supported_formats[0],
                                          .present_mode = Render::PresentModeFlagBits::FIFO,
                                          .extent = (surface_caps.extent_mode ==
                                                             Render::SurfaceExtentMode::Undefined ?
                                                         swapchain_extent :
                                                         surface_caps.current_extent)},
                .memory_allocation_size_hint = 1024 * 1024 * 16};

            if(surface_caps.extent_mode == Render::SurfaceExtentMode::Undefined)
            {
                if(!(swapchain_extent.width >= surface_caps.min_extent.width &&
                     swapchain_extent.height >= surface_caps.min_extent.height &&
                     swapchain_extent.width <= surface_caps.max_extent.width &&
                     swapchain_extent.height <= surface_caps.max_extent.height))
                {
                    throw std::runtime_error(
                        std::format("Cannot create swapchain with extent: {}.{}",
                                    swapchain_extent.width,
                                    swapchain_extent.height));
                }
            }

            device.reset(selected_dev->CreateDevice(device_info));
        }

        /*Render::Swapchain* swapchain = device->GetSwapchain();
        Render::Queue* queue =
            device->GetQueue(Render::QueueInfo{.family_index = present_queue_family, .index = 0});

        constexpr std::size_t FRAMES_COUNT = 3;

        std::array<std::unique_ptr<Render::Semaphore>, FRAMES_COUNT> acquire_semaphores;
        std::array<std::unique_ptr<Render::Fence>, FRAMES_COUNT> acquire_fences;
        std::array<bool, FRAMES_COUNT> acquire_fences_statuses;
        for(std::size_t i = 0; i < FRAMES_COUNT; i++)
        {
            acquire_semaphores[i] = std::unique_ptr<Render::Semaphore>(device->CreateSemaphore());
            acquire_fences[i] = std::unique_ptr<Render::Fence>(device->CreateFence());
            acquire_fences_statuses[i] = false;
        }

        std::size_t frame_index = 0;*/
        while(is_run)
        {
            win_sys->PollEvents();

            /*if(acquire_fences_statuses[frame_index] == true) //do wait
            {
                auto wait_fence = acquire_fences[frame_index].get();
                if(!wait_fence->Wait(std::numeric_limits<std::uint64_t>::max()))
                    throw std::runtime_error("Failed to wait on fence");

                wait_fence->Reset();
            }

            const Render::AcquireNextImageInfo next_image_info = {
                .semaphore = acquire_semaphores[frame_index].get(),
                .fence = acquire_fences[frame_index].get()};
            auto image_index_opt = swapchain->AcquireNextSwapchainImage(next_image_info);
            assert(image_index_opt);

            acquire_fences_statuses[frame_index] = true;

            Render::Semaphore* wait_sem = acquire_semaphores[frame_index].get();
            const Render::PresentInfo present_info = {.wait_semaphores = {&wait_sem, 1},
                                                      .queue = queue};
            bool present_res = swapchain->PresentSwapchainImage(present_info);
            assert(present_res);

            frame_index = (frame_index + 1) % FRAMES_COUNT;*/
        }
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "Undefined exception" << std::endl;
    }

    return 0;
}
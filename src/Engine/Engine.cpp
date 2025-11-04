#include "Engine.h"
#include <iostream>
#include <fstream>
#include "hrs/os.hpp"
#include "Core/Window/WindowSubsystem.h"
#include "Core/Window/OpenGL/OpenGLBackend.h"
#include "Core/Render/Context.h"
#include "Core/Window/GraphicWindow.h"

#define SHOW_INIT_ERROR(STR, ...) \
    if(!WindowSubsystem::ShowMessageBox(nullptr, \
                                        MessageBoxType::Error, \
                                        "Init failure", \
                                        STR __VA_OPT__(, __VA_ARGS__))) \
        std::cerr << STR __VA_OPT__(, __VA_ARGS__) << std::endl; // as fallback -> make logger!!!

Engine::Engine(int argc, char** argv)
{
    auto executable_path = hrs::exe_path();
    auto bin_path = executable_path.parent_path();
    auto root = (executable_path / ROOT_SUBDIR).lexically_normal();

    auto config_path = root / "config.json";
    std::ifstream config_ifs;
    config_ifs.open(config_path);
    if(!config_ifs.is_open())
        throw std::runtime_error(
            std::format("Failed to open config file: {}", config_path.string()));

    Core::Doc doc = Core::Doc::parse(config_ifs, nullptr, true, true);
    config = Core::Parse<Config>(doc);

    std::unique_ptr<RenderBackendInfo> backend_info;
    if(config.render_config.backend == RenderBackendType::OpenGL)
    {
        backend_info.reset(new OpenGLBackendInfo{
            RenderBackendInfo{.type = RenderBackendType::OpenGL},
            OpenGLBackendDefaultFramebufferInfo{.format = Render::Format::R8G8B8A8_UNORM},
            OpenGLVersion::OpenGL_4_6_Core,
            (config.render_config.enable_debug == true ? OpenGLBackendFlagBits::DebugContext :
                                                         OpenGLBackendFlagBits{})});
    }
    else
        throw std::runtime_error(
            std::format("Bad render backend type. Available backends: opengl"));

    instance = this;

    WindowSubsystem* win_sys = WindowSubsystem::Init();
    const GraphicWindowInfo window_info = GraphicWindowInfo{
        .resolution =
            WindowResolution{.width = static_cast<int>(config.engine_config.window.width),
                             .height = static_cast<int>(config.engine_config.window.height)},
        .title = config.engine_config.window.title.c_str()};

    window = win_sys->CreateGraphicWindow(window_info, *backend_info.get());

    auto render_impl_path =
        bin_path / hrs::decorate_shared_library_name(config.render_config.implementation);

    //for OpenGL
    std::array shader_resource_descs = {
        ResourceExtensionDesc<ShaderResourceDesc>{
            .ext = ".vert",
            .desc = ShaderResourceDesc{.stage = Render::ShaderStage::Vertex}},
        ResourceExtensionDesc<ShaderResourceDesc>{
            .ext = ".frag",
            .desc = ShaderResourceDesc{.stage = Render::ShaderStage::Fragment}},
    };

    const RenderEngineInfo render_engine_info = RenderEngineInfo{.resource_set_count = 3,
                                                                 .implementation = render_impl_path,
                                                                 .window = window};
    render_engine =
        std::unique_ptr<RenderEngine>(new TaskRootWrapper<RenderEngine>(render_engine_info));

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

    const ResourceManagerInfo resource_manager_info = ResourceManagerInfo{
        .shaders_path_prefix = root / "game/shaders/ogl/compiled",
        .images_path_prefix = root / "game/images",
        .shader_resource_descs = shader_resource_descs,
        .transfer_storage_info = {.buffer_size = config.engine_config.transfer_storage_size}};
    resource_manager = std::unique_ptr<ResourceManager>(new ResourceManager(resource_manager_info));
}

Engine::~Engine()
{}

int Engine::Enter(int argc, char** argv)
{
    int res = 0;
    try
    {
        Engine engine(argc, argv);
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

const Timer& Engine::GetTimer() const noexcept
{
    return frame_timer;
}

float Engine::GetUpdateFactor() const noexcept
{
    return update_factor;
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

    update_factor = config.engine_config.reference_update_factor;

    auto max_fps_duration = Timer::DurationFromFPS(config.engine_config.max_fps);

    bool is_run = true;
    Events::Connect<WindowCloseEvent>(window,
                                      [&is_run](const WindowCloseEvent&)
                                      {
                                          is_run = false;

                                          return Events::HandlerAction::Erase;
                                      });

    WindowSubsystem* win_sys = WindowSubsystem::GetSubsystem();

    static_cast<OpenGLBackend*>(window->GetRenderBackend())
        ->SetPresentMode(WindowPresentMode::Immediate);

    render_engine->DEBUG_DrawGraph(std::cerr);
    {
        std::ofstream ofs("graph.dot");
        render_engine->DEBUG_DrawGraph(ofs);
    }

    while(is_run)
    {
        frame_timer.Begin();

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

        update_factor =
            frame_timer.AdjustUpdateFactor(config.engine_config.reference_update_factor);

        auto title =
            std::format("FPS = {:4.3f}, frametime = {:2.3f}ms, update_factor = {:2.3f}",
                        frame_timer.GetFPS(),
                        std::chrono::duration<float, std::milli>(frame_timer.GetDuration()).count(),
                        update_factor);
        window->SetTitle(title.c_str());

        frame_index++;
    }
}
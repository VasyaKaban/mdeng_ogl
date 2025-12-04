#include "Game.h"
#include "Engine/Engine.h"
#include <fstream>
#include "Core/Window/WindowSubsystem.h"
#include "Core/Window/OpenGL/OpenGLBackend.h"
#include "Engine/ResourceManager/ResourceManager.h"
#include "Engine/RenderEngine/RenderEngine.h"
#include "Core/Render/Resolve.h"
#include "Core/Render/Context.h"

constexpr static std::uint16_t RESOURCE_SET_COUNT = 3;

extern "C" Engine::GameResolve* ResolveGame()
{
    static GameResolve resolve;

    return &resolve;
}

GameResolve::GameResolve()
{}

GameResolve::~GameResolve()
{}

Engine::GameEngineInfo GameResolve::Init()
{
    auto root = Engine::GameEngine::GetRootFolder();
    auto game_folder = Engine::GameEngine::GetGameFolder();

    auto config_path =
        Engine::GameEngine::GetUserDataFolder() / CONFIG_NAME; //1. check userdata folder
    std::ifstream config_ifs;
    config_ifs.open(config_path);
    if(!config_ifs.is_open())
    {
        config_path = root / CONFIG_NAME; //2. check root folder
        config_ifs.open(config_path);
        if(!config_ifs.is_open())
            throw std::runtime_error(
                std::format("Failed to open config file: {}", config_path.string()));
    }

    auto win_sys = Core::WindowSubsystem::GetSubsystem();

    JSON::Doc doc = JSON::Doc::parse(config_ifs, nullptr, true, true);
    config = JSON::Parse<Config>(doc);

#pragma message("Query for available formats!")

    Core::RenderBackendInfo* backend_info = nullptr;
    std::variant<Core::OpenGLBackendInfo> backend_impl;
    if(config.render.backend == Core::RenderBackendType::OpenGL)
    {
        backend_impl = Core::OpenGLBackendInfo{
            Core::RenderBackendInfo{.type = Core::RenderBackendType::OpenGL},
            Core::OpenGLBackendDefaultFramebufferInfo{.format = Render::Format::R8G8B8A8_UNORM},
            Core::OpenGLVersion::OpenGL_4_6_Core,
            (config.render.enable_debug == true ? Core::OpenGLBackendFlagBits::DebugContext :
                                                  Core::OpenGLBackendFlagBits{})};

        backend_info = &std::get<Core::OpenGLBackendInfo>(backend_impl);
    }
    else
        throw std::runtime_error(
            std::format("Bad render backend type. Available backends: opengl"));

    const Core::GraphicWindowInfo window_info = {
        .resolution =
            Core::WindowResolution{.width = static_cast<int>(config.engine.window.width),
                                   .height = static_cast<int>(config.engine.window.height)},
        .title = config.engine.window.title.c_str()};

    auto window = win_sys->CreateGraphicWindow(window_info, *backend_info);

    auto render_impl_path = Engine::GameEngine::GetBinFolder() /
                            hrs::decorate_shared_library_name(config.render.implementation);

    Engine::RenderEngineState render_engine_state(render_impl_path);
    render_engine_state.Init(window->GetRenderBackend());
    auto available_contexts = render_engine_state.GetResolve()->GetAvailableContexts();
    if(available_contexts.empty())
        throw std::runtime_error("No available contexts found");

    //render queue
    //transfer queue
    //present queue

#pragma message("Change!!!")
    std::vector<float> queue_priorities = {1};
    Render::SelectedContextDesc selected_ctx = {
        .index = 0,
        .queue_family_infos = {Render::QueueFamilyInfo{.index = 0,
                                                       .queue_count = 1,
                                                       .queue_priorities = queue_priorities}}};

    const Engine::RenderEngineInfo render_engine_info = {.resource_set_count = RESOURCE_SET_COUNT,
                                                         .selected_context = selected_ctx,
                                                         .window = window};

    auto render_engine = std::unique_ptr<Engine::RenderEngine>(
        new Engine::TaskRootWrapper<Engine::RenderEngine>(render_engine_info,
                                                          std::move(render_engine_state)));

    if(config.render.enable_debug)
    {
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

                std::cerr
                    << std::format("[{}][{}]({}) -> {}\n", severity_name, type_name, id, message);
            }};
        render_engine->GetContext()->SetDebugMessenger(debug_messenger_info);
    }

    //for OpenGL
    std::array shader_resource_descs = {
        Engine::ResourceExtensionDesc<Engine::ShaderResourceDesc>{
            .ext = ".vert",
            .desc = Engine::ShaderResourceDesc{.stage = Render::ShaderStageFlagBits::Vertex}},
        Engine::ResourceExtensionDesc<Engine::ShaderResourceDesc>{
            .ext = ".frag",
            .desc = Engine::ShaderResourceDesc{.stage = Render::ShaderStageFlagBits::Fragment}},
    };

#pragma message("Change queue family!")
    const Engine::ResourceManagerInfo resource_manager_info = {
        .shaders_path_prefix = game_folder / "shaders/ogl/compiled",
        .images_path_prefix = game_folder / "images",
        .shader_resource_descs = shader_resource_descs,
        .render_engine = render_engine.get(),
        .queue_family_index = 0,
        .transfer_storage_info = {.buffer_size = config.engine.transfer_storage_size,
                                  .queue_info = Render::QueueInfo{.family_index = 0, .index = 0}}};

    auto resource_manager = std::unique_ptr<Engine::ResourceManager>(
        new Engine::ResourceManager(resource_manager_info));

    return Engine::GameEngineInfo{.window = window,
                                  .render_engine = render_engine.release(),
                                  .resource_manager = resource_manager.release(),
                                  .max_fps = config.engine.max_fps,
                                  .reference_update_factor = config.engine.reference_update_factor};
}

Engine::Game* GameResolve::CreateGame()
{
    return new Game;
}

void GameResolve::operator delete(void* ptr) noexcept
{
    //noop
}

Game::Game()
{}

Game::~Game()
{}
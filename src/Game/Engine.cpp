#include "Engine.h"
#include <iostream>
#include "Core/Window/WindowSubsystem.h"
#include "Core/Window/OpenGL/OpenGLBackend.h"
#include "Core/Window/GraphicWindow.h"
#include "OpenGLRenderBackend/Context/Context.h"
#include "RenderEngine/RenderQueue/RenderQueue.h"

#define SHOW_INIT_ERROR(STR, ...) \
    if(!WindowSubsystem::ShowMessageBox(nullptr, \
                                        MessageBoxType::Error, \
                                        "Init failure", \
                                        STR __VA_OPT__(, __VA_ARGS__))) \
        std::cerr << STR __VA_OPT__(, __VA_ARGS__) << std::endl; // as fallback -> make logger!!!

Engine::Engine()
{
    instance = this;

    WindowSubsystem* win_sys = WindowSubsystem::Init();
    const GraphicWindowInfo window_info =
        GraphicWindowInfo{.resolution = WindowResolution{.width = 800, .height = 600},
                          .title = "game"};

    const OpenGLBackendInfo render_info =
        OpenGLBackendInfo{RenderBackendInfo{.type = RenderBackendType::OpenGL},
                          OpenGLBackendDefaultFramebufferInfo{.red_channel_bits = 8,
                                                              .green_channel_bits = 8,
                                                              .blue_channel_bits = 8,
                                                              .alpha_channel_bits = 0,
                                                              .depth_channel_bits = 0,
                                                              .stencil_channel_bits = 0},
                          OpenGLVersion::OpenGL_4_6_Core};

    window = win_sys->CreateGraphicWindow(window_info, render_info);

    std::unique_ptr<OpenGL::Context> _context(
        new OpenGL::Context(static_cast<OpenGLBackend*>(window->GetRenderBackend())));

    /*
    FS:
    
    bin:
        exe
        dll
        ...

    game:
        shaders
        images

    config.json/toml
    
    */
    std::array shader_resource_descs = {
        ResourceExtensionDesc<ShaderResourceDesc>{
            .ext = ".vert",
            .desc = ShaderResourceDesc{.stage = Render::ShaderStage::Vertex}},
        ResourceExtensionDesc<ShaderResourceDesc>{
            .ext = ".frag",
            .desc = ShaderResourceDesc{.stage = Render::ShaderStage::Fragment}}};

    const ResourceManagerInfo resource_manager_info =
        ResourceManagerInfo{.shaders_path_prefix = "game/shaders",
                            .images_path_prefix = "game/images",
                            .shader_resource_descs = shader_resource_descs};
    resource_manager = std::unique_ptr<ResourceManager>(new ResourceManager(resource_manager_info));

    const RenderEngineInfo render_engine_info =
        RenderEngineInfo{.resource_set_count = 3,
                         .transfer_channel_info = TransferChannelInfo{.pool_block_size = 8192,
                                                                      .pool_blocks_reserve = 16}};
    render_engine =
        std::unique_ptr<RenderEngine>(new RenderEngine(render_engine_info, std::move(_context)));
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

void Engine::loop()
{
    bool is_run = true;
    auto close_handler = [&is_run](const EventHandlers::WindowCloseEvent&)
    {
        is_run = false;
    };

    window->GetEventHandlers().AddHandler(std::move(close_handler));

    WindowSubsystem* win_sys = WindowSubsystem::GetSubsystem();

    while(is_run)
    {
        render_engine->AcquireNextResourceSet();

        render_engine->GetContext()->AcquireNextSwapchainImage(
            render_engine->GetRenderQueue()->GetCurrentSwapchainWaitSemaphore());

        win_sys->PollEvents();

        EvaluateDesc desc = {.cmd = nullptr, .pipeline = nullptr};
        render_engine->Evaluate(desc);

        Render::Semaphore* present_wait_sem =
            render_engine->GetRenderQueue()->GetCurrentSignalSemaphore();
        Render::PresentInfo present_info = {.wait_semaphores = {&present_wait_sem, 1}};
        render_engine->GetContext()->ReleaseSwapchainImage(present_info);
    }
}

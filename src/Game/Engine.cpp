#include "Engine.h"
#include <iostream>
#include "Core/Window/WindowSubsystem.h"
#include "Core/Window/OpenGL/OpenGLBackend.h"
#include "Core/Window/GraphicWindow.h"
#include "OpenGLRenderBackend/Context/Context.h"
#include "RenderEngine/RenderQueue/RenderQueue.h"

#include "RenderEngine/Tasks/RenderPassTask.h"
class TestRenderPass : public RenderPassTask
{
    constexpr static Render::RenderPassInfo get_info()
    {
        Render::AttachmentDescription desc = {.clear_load = true};
        return Render::RenderPassInfo{.color_attachment_descriptions = {&desc, 1},
                                      .depth_stencil_attachment_description = nullptr};
    }
public:
    TestRenderPass(Task<RenderQueue>* _parent, TaskKey&& key)
        : RenderPassTask(_parent, std::move(key), get_info())
    {}

    virtual ~TestRenderPass() override
    {}

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) override
    {
        constexpr std::array colors = {
            Render::ClearColorValue{.value = Render::ClearColorFloatValue{0.0f, 0.0f, 0.0f, 0.0f}},
            Render::ClearColorValue{.value = Render::ClearColorFloatValue{1.0f, 0.0f, 0.0f, 0.0f}},
            Render::ClearColorValue{.value = Render::ClearColorFloatValue{0.0f, 1.0f, 0.0f, 0.0f}},
            Render::ClearColorValue{.value = Render::ClearColorFloatValue{0.0f, 0.0f, 1.0f, 0.0f}},
            Render::ClearColorValue{.value = Render::ClearColorFloatValue{1.0f, 1.0f, 0.0f, 0.0f}},
            Render::ClearColorValue{.value = Render::ClearColorFloatValue{1.0f, 0.0f, 1.0f, 0.0f}},
            Render::ClearColorValue{.value = Render::ClearColorFloatValue{0.0f, 1.0f, 1.0f, 0.0f}},
            Render::ClearColorValue{.value = Render::ClearColorFloatValue{1.0f, 1.0f, 1.0f, 0.0f}}};

        static std::size_t i = 0;
        static auto prev = std::chrono::system_clock::now();

        auto now = std::chrono::system_clock::now();
        if(std::chrono::duration<float>(now - prev).count() >= 1)
        {
            i = (i + 1) % colors.size();

            prev = now;
        }

        const Render::RenderPassBeginInfo info = {
            .framebuffer = parent->GetRoot()->GetContext()->GetCurrentDefaultFramebuffer(),
            .clear_color_values = {&colors[i], 1},
            .clear_depth_stencil_value = Render::ClearDepthStencilValue{}};
        handle->Begin(eval_desc.cmd, info);

        return eval_desc;
    }

    virtual void End(const EvaluateDesc& eval_desc) override
    {
        handle->End(eval_desc.cmd);
    }
};

CHECK_TASK_IS_READY(TestRenderPass)

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

rpasses:
    deffered(deffred buffers) 
        -> {deffered_resolve(out image) -> ... -> screenshot(in: image from prev; out: image) -> default framebuffer resolve} <- "post-process chain" + share same state for out and in images
            -> default framebuffer resolve

rpasses:
    GEOMETRY:
        deffered
    POST-PROCESS[post-prrocess state(const Image* input; const Image* output)]:
        deffered resolve -> ... -> screenshot -> default framebuffer resolve

    FULL:///
    GEOMETRY:
        deffered
    POST-PROCESS-GEOMETRY[post-prrocess state(const Image* input; const Image* output)]:
        deffered resolve -> ...

    <post-geometry-state>

    GUI GEOMETRY:
        render
    GUI POST-PROCESS
       ...

    <post gui state>
    screenshot -> default framebuffer resolve


    class AttachmentChain:
        chain*
        

    TestRenderPass(state ref) -> ScreenShot(state ref) -> Resolve[state]
    
    */
    std::array shader_resource_descs = {
        ResourceExtensionDesc<ShaderResourceDesc>{
            .ext = ".vert",
            .desc = ShaderResourceDesc{.stage = Render::ShaderStage::Vertex}},
        ResourceExtensionDesc<ShaderResourceDesc>{
            .ext = ".frag",
            .desc = ShaderResourceDesc{.stage = Render::ShaderStage::Fragment}}};

    const RenderEngineInfo render_engine_info = RenderEngineInfo{.resource_set_count = 3};
    render_engine = std::unique_ptr<Task<RenderEngine>>(
        new Task<RenderEngine>(render_engine_info, std::move(_context)));

    const ResourceManagerInfo resource_manager_info = ResourceManagerInfo{
        .shaders_path_prefix = "game/shaders",
        .images_path_prefix = "game/images",
        .shader_resource_descs = shader_resource_descs,
        .transfer_channel_state_info =
            TransferChannelStateInfo{.pool_block_size = 8192, .pool_blocks_reserve = 16}};
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

void Engine::loop()
{
    auto test = new Task<TestRenderPass>(
        render_engine->GetRenderQueue(),
        TaskBase::TaskKey{.priority = 0, .name = std::string_view("Test")});

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
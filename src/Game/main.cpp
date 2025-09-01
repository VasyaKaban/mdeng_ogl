#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include "hrs/size_literals.hpp"
#include "Core/Window/WindowSubsystem.h"
#include "Core/Window/GraphicWindow.h"
#include "Core/DDS/DDS.h"
#include "Core/Wavefront/WFObject.h"
#include "RenderEngine/RenderEngine.h"
#include "Core/Render/Context/Context.h"
#include "RenderEngine/RenderQueue/RenderQueue.h"
#include "hrs/registry.hpp"
#include "Camera/PerspectiveCamera.h"

class AA
{
    inline static bool n = []()
    {
        std::cout << "Amogus";
        return false;
    }();
};

struct some_info
{
    int i;
    float f;
    char c;
};

struct base
{
    virtual ~base() = 0;
    virtual double get() const noexcept = 0;
};

inline base::~base()
{}

struct der_i : public base
{
    int i;

    der_i(int _i)
        : i(_i)
    {}
    virtual ~der_i() override = default;

    virtual double get() const noexcept override
    {
        return i;
    }
};

struct der_f : public base
{
    float f;

    der_f(float _f)
        : f(_f)
    {}
    virtual ~der_f() override = default;

    virtual double get() const noexcept override
    {
        return f;
    }
};

void entry()
{
    PerspectiveCamera camera;
    camera.GetOffset() = hrs::math::glsl::vec3(1, 2, 3);
    auto& perspective = camera.GetPerspective();
    perspective.aspect = static_cast<float>(800) / 600;
    perspective.far = 100;
    perspective.near = 1;
    perspective.fov = hrs::math::deg_to_rad(75);

    auto& rotation = camera.GetRotation();
    rotation.yaw = hrs::math::deg_to_rad(90);
    rotation.pitch = hrs::math::deg_to_rad(-90);
    rotation.roll = hrs::math::deg_to_rad(90);

    camera.SignalUpdated();
    camera.Build();

    /*hrs::registry<base, some_info> registry;
    registry.register_class("der_i",
                            [](const some_info& info)
                            {
                                return std::make_unique<der_i>(info.i);
                            });

    registry.register_class("der_f",
                            [](const some_info& info)
                            {
                                return std::make_unique<der_f>(info.f);
                            });

    some_info info = {.i = 1, .f = 3.14f, .c = '\0'};
    auto der_i_instance = registry.create_instance("der_i", info);
    auto der_f_instance = registry.create_instance("der_f", info);
    auto der_c_instance = registry.create_instance("der_c", info);

    std::cerr << std::filesystem::current_path() << std::endl;*/

    auto window_subsystem = WindowSubsystem::Init();

    const GraphicWindowInfo window_info = {.resolution =
                                               WindowResolution{.width = 800, .height = 600},
                                           .title = "game"};

    OpenGLBackendInfo render_info = {RenderBackendInfo{.type = RenderBackendType::OpenGL},
                                     OpenGLBackendDefaultFramebufferInfo{.red_channel_bits = 8,
                                                                         .green_channel_bits = 8,
                                                                         .blue_channel_bits = 8,
                                                                         .alpha_channel_bits = 8,
                                                                         .depth_channel_bits = 0,
                                                                         .stencil_channel_bits = 0},
                                     OpenGLVersion::OpenGL_4_6_Core};

    auto window = window_subsystem->CreateGraphicWindow(window_info, render_info);

    std::unique_ptr<Context> context(
        new Context(static_cast<OpenGLBackend*>(window->GetRenderBackend())));

    {
        using namespace hrs::size_literals;

        std::array shader_resource_descs = {
            ResourceExtensionDesc<ShaderResourceDesc>{
                .ext = "vert",
                .desc = ShaderResourceDesc{.stage = ShaderStage::Vertex}},
            ResourceExtensionDesc<ShaderResourceDesc>{
                .ext = "frag",
                .desc = ShaderResourceDesc{.stage = ShaderStage::Fragment}}};

        const RenderEngineInfo render_engine_info = {
            .resource_set_count = 3,
            .transfer_channel_info = TransferChannelInfo{.size = 2'304'000,
                                                         .regions_block_size = 8192,
                                                         .regions_reserve = 0},
            .resource_manager_info =
                ResourceManagerInfo{.shader_path_prefix = std::filesystem::path("game") / "shaders",
                                    .image_path_prefix = std::filesystem::path("game") / "images",
                                    .shader_resource_descs = shader_resource_descs}};
        RenderEngine::Init(render_engine_info, std::move(context));
    }

    RenderEngine* render_engine = RenderEngine::GetInstance();
    ResourceManager* resource_manager = render_engine->GetResourceManager();

    auto image = resource_manager->CreateImage("test_bc3_dx10.dds", true);

    bool is_run = true;
    auto close_event_handler = window->GetEventHandlers().AddHandler(
        [&is_run](const EventHandlers::WindowCloseEvent&) -> void
        {
            is_run = false;
        });

    bool image_ready = false;
    while(is_run)
    {
        window_subsystem->PollEvents();

        render_engine->AcquireNextResourceSet();
        render_engine->GetContext()->AcquireNextSwapchainImage(
            render_engine->GetRenderQueue()->GetCurrentSwapchainWaitSemaphore());

        EvaluateDesc eval_desc = {.cmd = nullptr, .pipeline = nullptr};
        render_engine->Evaluate(eval_desc);

        if(image->GetState() == ResourceManager::ImageEntryState::Ready && !image_ready)
        {
            image_ready = true;
            std::cerr << "Image is ready on: " << render_engine->GetCurrentResourceSetIndex()
                      << std::endl;
        }

        Semaphore* render_sempahore = render_engine->GetRenderQueue()->GetCurrentSignalSemaphore();
        PresentInfo present_info = {.wait_semaphores = {&render_sempahore, 1}};
        render_engine->GetContext()->ReleaseSwapchainImage(present_info);
    }

    //WFObject obj("../game/objects/stalker_freedom.obj", "../game/objects");
}

int main(int argc, char** argv)
{
    //RGBA DXT10 +
    //BC DXT10 +
    //RGAB PF +
    //BC PF +

    // auto dds_data = ReadFile(R"(C:\Users\Lenovo\Desktop\red_moon_desert.dds)");
    /*struct Data
    {
        std::filesystem::path path;
        DDS::DDSResult result;
        DDS::ResolveResult resolved;
    };

    std::vector<Data> inputs = {
        //Data{.path = R"(C:\Users\Lenovo\Desktop\test_rgba_pf.dds)"},
        //Data{.path = R"(C:\Users\Lenovo\Desktop\test_rgba_dx10.dds)"},
        Data{.path = R"(C:\Users\Lenovo\Desktop\test_bc3_pf.dds)"},
        //Data{.path = R"(C:\Users\Lenovo\Desktop\test_bc3_dx10.dds)"},
        //Data{.path = R"(C:\Users\Lenovo\Desktop\test_rg_pf.dds)"},
        //Data{.path = R"(C:\Users\Lenovo\Desktop\test_rg_pf2.dds)"},
        //Data{.path = R"(C:\Users\Lenovo\Desktop\test_r_pf.dds)"},
        //Data{.path = R"(C:\Users\Lenovo\Desktop\test_r32_pf.dds)"},
    };

    for(auto& data: inputs)
    {
        auto dds_data = ReadFile(data.path);
        data.result = DDS::Parse(dds_data);
        data.resolved = DDS::Resolve(data.result);
    }*/

    try
    {
        entry();
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
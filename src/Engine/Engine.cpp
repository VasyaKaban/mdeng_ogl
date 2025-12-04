#include "Engine.h"
#include <iostream>
#include <fstream>
#include "Core/Window/WindowSubsystem.h"
#include "Core/Window/OpenGL/OpenGLBackend.h"
#include "Core/Render/Context.h"
#include "Core/Window/GraphicWindow.h"

namespace Engine
{
#define SHOW_INIT_ERROR(STR, ...) \
    if(!Core::WindowSubsystem::ShowMessageBox(nullptr, \
                                              Core::MessageBoxType::Error, \
                                              "Init failure", \
                                              STR __VA_OPT__(, __VA_ARGS__))) \
        std::cerr << STR __VA_OPT__(, __VA_ARGS__) << std::endl; // as fallback -> make logger!!!

    GameEngine::GameEngine(const GameEngineInfo& info, GameState&& _game_state)
        : game_state(std::move(_game_state)),
          window(info.window),
          render_engine(info.render_engine),
          resource_manager(info.resource_manager),
          frame_index(0),
          frame_timer(),
          max_fps(info.max_fps),
          reference_update_factor(info.reference_update_factor),
          update_factor(0)
    {
        instance = this;

        game.reset(game_state.GetResolve()->CreateGame());
    }

    GameEngine::~GameEngine()
    {}

    int GameEngine::Enter(int argc, char** argv)
    {
        int res = 0;
        try
        {
            Core::WindowSubsystem::Init();

            std::string_view game_impl;
            if(argc == 1) //no game
                game_impl = DEFAULT_GAME_IMPLEMENTATION_NAME;
            else
                game_impl = argv[1];

            GameState _game_state(GetBinFolder() / hrs::decorate_shared_library_name(game_impl));
            auto engine_info = _game_state.Init();

            GameEngine engine(engine_info, std::move(_game_state));

            instance->loop();
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

        Core::WindowSubsystem::Close();

        return res;
    }

    GameEngine* GameEngine::GetInstance() noexcept
    {
        return instance;
    }

    std::filesystem::path GameEngine::GetRootFolder()
    {
        return (hrs::exe_path() / ROOT_SUBDIR).lexically_normal();
    }

    std::filesystem::path GameEngine::GetBinFolder()
    {
        return hrs::exe_path().parent_path().lexically_normal();
    }

    std::filesystem::path GameEngine::GetGameFolder()
    {
        return (hrs::exe_path() / GAME_SUBDIR).lexically_normal();
    }

    std::filesystem::path GameEngine::GetUserDataFolder()
    {
        return (hrs::exe_path() / USERDATA_SUBDIR).lexically_normal();
    }

    Core::GraphicWindow* GameEngine::GetWindow() const noexcept
    {
        return window;
    }

    RenderEngine* GameEngine::GetRenderEngine() const noexcept
    {
        return render_engine.get();
    }

    ResourceManager* GameEngine::GetResourceManager() const noexcept
    {
        return resource_manager.get();
    }

    std::uint64_t GameEngine::GetFrameIndex() const noexcept
    {
        return frame_index;
    }

    const Timer& GameEngine::GetTimer() const noexcept
    {
        return frame_timer;
    }

    std::uint32_t GameEngine::GetMaxFPS() const noexcept
    {
        return max_fps;
    }

    float GameEngine::GetUpdateFactor() const noexcept
    {
        return update_factor;
    }

#include <fstream>

    void GameEngine::loop()
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

        update_factor = reference_update_factor;

        auto max_fps_duration = Timer::DurationFromFPS(max_fps);

        bool is_run = true;
        Events::Connect<Core::WindowCloseEvent>(window,
                                                [&is_run](const Core::WindowCloseEvent&)
                                                {
                                                    is_run = false;

                                                    return Events::HandlerAction::Erase;
                                                });

        Core::WindowSubsystem* win_sys = Core::WindowSubsystem::GetSubsystem();

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

            update_factor = frame_timer.AdjustUpdateFactor(reference_update_factor);

            auto title = std::format(
                "FPS = {:4.3f}, frametime = {:2.3f}ms, update_factor = {:2.3f}",
                frame_timer.GetFPS(),
                std::chrono::duration<float, std::milli>(frame_timer.GetDuration()).count(),
                update_factor);
            window->SetTitle(title.c_str());

            frame_index++;
        }
    }
};
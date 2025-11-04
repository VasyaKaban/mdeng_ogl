#pragma once

#include "hrs/os.hpp"
#include "Core/Window/GraphicWindow.h"
#include "RenderEngine/RenderEngine.h"
#include "ResourceManager/ResourceManager.h"
#include "Timer/Timer.h"
#include "Config/Config.h"

class Engine : hrs::non_copyable, hrs::non_movable
{
    Engine(int argc, char** argv);
    ~Engine();
public:
    static int Enter(int argc, char** argv);
    static Engine* GetInstance() noexcept;

    GraphicWindow* GetWindow() const noexcept;
    ResourceManager* GetResourceManager() const noexcept;
    RenderEngine* GetRenderEngine() const noexcept;

    std::uint64_t GetFrameIndex() const noexcept;
    const Timer& GetTimer() const noexcept;

    float GetUpdateFactor() const noexcept;
private:
    void loop();
private:
    constexpr static float DEFAULT_REFERENCE_UPDATE_FACTOR = 120;

    static inline Engine* instance = nullptr;

    Config config;

    GraphicWindow* window;

    std::unique_ptr<RenderEngine> render_engine;
    std::unique_ptr<ResourceManager> resource_manager;

    std::uint64_t frame_index;
    Timer frame_timer;

    float update_factor;
};
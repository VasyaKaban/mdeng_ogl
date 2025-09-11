#pragma once

#include "Core/Window/GraphicWindow.h"
#include "RenderEngine/RenderEngine.h"
#include "ResourceManager/ResourceManager.h"

class Engine : hrs::non_copyable, hrs::non_movable
{
    Engine();
    ~Engine();
public:
    static int Enter(int argc, char** argv);
    static Engine* GetInstance() noexcept;

    GraphicWindow* GetWindow() const noexcept;
    ResourceManager* GetResourceManager() const noexcept;
    RenderEngine* GetRenderEngine() const noexcept;
private:
    void loop();
private:
    static inline Engine* instance = nullptr;

    GraphicWindow* window;
    std::unique_ptr<ResourceManager> resource_manager;
    std::unique_ptr<Task<RenderEngine>> render_engine;
};
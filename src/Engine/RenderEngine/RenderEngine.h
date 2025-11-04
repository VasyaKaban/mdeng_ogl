#pragma once

#include "hrs/os.hpp"
#include "TaskTree/Task.h"
#include "Core/Window/GraphicWindow.h"

struct RenderEngineInfo
{
    std::uint16_t resource_set_count;
    std::filesystem::path implementation;
    GraphicWindow* window;
};

class RenderEngine : public TaskRoot, public Events::EventListener<WindowResizedEvent>
{
public:
    RenderEngine(const RenderEngineInfo& info);
    virtual ~RenderEngine() override;

    std::uint16_t GetResourceSetCount() const noexcept;
    std::uint16_t GetCurrentResourceSetIndex() const noexcept;
    std::uint16_t GetPreviousResourceSetIndex() const noexcept;
    std::uint16_t GetNextResourceSetIndex() const noexcept;

    void AcquireNextResourceSet() noexcept;

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) override;
    virtual void End(const EvaluateDesc& eval_desc) override;

    Render::Context* GetContext() const noexcept;
private:
    Events::HandlerAction Handle(const WindowResizedEvent& event);
private:
    hrs::dynamic_library lib;
    Render::RenderResolve resolve;

    std::unique_ptr<Render::Context> context;

    std::uint16_t resource_set_count;
    std::uint16_t current_resource_set_index;
};
#pragma once

#include "hrs/os.hpp"
#include "TaskTree/Task.h"
#include "Core/Window/GraphicWindow.h"

namespace Engine
{
    class RenderEngine;

    class RenderEngineState : hrs::non_copyable
    {
    public:
        RenderEngineState(const std::filesystem::path& implementation_path);
        ~RenderEngineState() = default;
        RenderEngineState(RenderEngineState&&) = default;
        RenderEngineState& operator=(RenderEngineState&&) = default;

        void Init(Core::RenderBackend* backend);

        Render::Resolve* GetResolve() const noexcept;
    private:
        hrs::dynamic_library lib;
        std::unique_ptr<Render::Resolve> resolve;
    };

    struct RenderEngineInfo
    {
        std::uint16_t resource_set_count;
        Render::SelectedContextDesc selected_context;
        Core::GraphicWindow* window;
    };

    class RenderEngine : public TaskRoot, public Events::EventListener<Core::WindowResizedEvent>
    {
    public:
        RenderEngine(const RenderEngineInfo& info, RenderEngineState&& _state);
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
        Events::HandlerAction Handle(const Core::WindowResizedEvent& event);
    private:
        RenderEngineState state;
        std::unique_ptr<Render::Context> context;

        std::uint16_t resource_set_count;
        std::uint16_t current_resource_set_index;
    };
};
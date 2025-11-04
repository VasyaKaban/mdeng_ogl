#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Pipeline.h"
#include "GraphicsPipelineState.h"

namespace OpenGL
{
    class Pipeline : public Render::Pipeline, hrs::non_copyable, hrs::non_movable
    {
    public:
        Pipeline(Context* _parent, const Render::GraphicsPipelineInfo& info);
        Pipeline(Context* _parent, const Render::ComputePipelineInfo& info);

        virtual ~Pipeline() override;

        GLHandle GetHandle() const noexcept;
        GraphicsPipelineState* GetGraphicsPipelineState() noexcept;

        virtual Render::Context* GetContext() const noexcept override;
    private:
        Context* parent;
        GLHandle handle;

        GraphicsPipelineState* graphics_state;
    };
};
#pragma once

#include <vector>
#include <optional>
#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/RenderPass.h"

namespace OpenGL
{
    class RenderPass : public Render::RenderPass, hrs::non_copyable, hrs::non_movable
    {
    public:
        RenderPass(Device* _parent, const Render::RenderPassInfo& info);
        virtual ~RenderPass() override;

        virtual Render::Device* GetParent() const noexcept override;

        void Begin(CommandBuffer& cmd, const Render::RenderPassBeginInfo& info);
    private:
        Device* parent;

        struct ColorAttachmentDescription
        {
            Render::FormatType type;
            bool clear;
        };

        struct DepthStencilAttachmentDescription
        {
            bool clear_depth;
            bool clear_stencil;
        };

        std::vector<ColorAttachmentDescription> color_attachment_descriptions;
        std::optional<DepthStencilAttachmentDescription> depth_stencil_attachment_description;
    };
};
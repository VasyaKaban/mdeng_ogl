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
        RenderPass(Context* _parent, const Render::RenderPassInfo& info);
        virtual ~RenderPass() override;

        void Begin(CommandBuffer& cmd, const Render::RenderPassBeginInfo& info);

        virtual Render::Context* GetContext() const noexcept override;
    private:
        Context* parent;

        struct ClearAttachmentDescription
        {
            Render::ColorAttachment desc;
            GLuint index;
        };

        std::vector<ClearAttachmentDescription> clear_color_attachment_descriptions;
        std::optional<Render::DepthStencilAttachment> clear_depth_stencil_attachment_description;
    };
};
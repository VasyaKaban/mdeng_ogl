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

        virtual void Begin(const Render::CommandBuffer* cmd,
                           const Render::RenderPassBeginInfo& info) override;
        virtual void End(const Render::CommandBuffer* cmd) override;

        virtual Render::Context* GetContext() const noexcept override;
    private:
        Context* parent;

        struct ClearAttachmentDescription
        {
            Render::AttachmentDescription desc;
            GLuint index;
        };

        std::vector<ClearAttachmentDescription> clear_color_attachment_descriptions;
        std::optional<Render::AttachmentDescription> clear_depth_stencil_attachment_description;
        //GLHandle handle; no handle at all!!!
    };
};
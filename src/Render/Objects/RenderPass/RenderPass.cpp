#include "RenderPass.h"
#include "../../Context/Context.h"
#include "../Framebuffer/Framebuffer.h"

RenderPass::RenderPass() noexcept
    : parent(nullptr)
{}

RenderPass::RenderPass(Context* _parent, const RenderPassInfo& info)
    : parent(_parent)
{
    clear_color_attachment_descriptions.reserve(info.color_attachment_descriptions.size());
    for(std::size_t i = 0; i < info.color_attachment_descriptions.size(); i++)
    {
        const auto& att = info.color_attachment_descriptions[i];
        if(att.clear_load)
            clear_color_attachment_descriptions.push_back(
                ClearAttachmentDescription{.desc = att, .index = static_cast<GLuint>(i)});
    }

    if(info.depth_stencil_attachment_description &&
       info.depth_stencil_attachment_description->clear_load)
        clear_depth_stencil_attachment_description = *info.depth_stencil_attachment_description;
}

RenderPass::~RenderPass()
{
    destroy();
}

RenderPass::RenderPass(RenderPass&& rpass) noexcept
    : parent(rpass.parent),
      clear_color_attachment_descriptions(std::move(rpass.clear_color_attachment_descriptions)),
      clear_depth_stencil_attachment_description(
          std::move(rpass.clear_depth_stencil_attachment_description))
{}

RenderPass& RenderPass::operator=(RenderPass&& rpass) noexcept
{
    destroy();

    parent = rpass.parent;
    clear_color_attachment_descriptions = std::move(rpass.clear_color_attachment_descriptions);
    clear_depth_stencil_attachment_description =
        std::move(rpass.clear_depth_stencil_attachment_description);

    return *this;
}

void RenderPass::Begin(const RenderPassBeginInfo& info)
{
    //bind framebuffer
    parent->GetLoader().BindFramebuffer(GL_DRAW_FRAMEBUFFER, info.framebuffer->handle);

    //clear attachments
    for(const auto& clear_color_att: clear_color_attachment_descriptions)
    {
        auto& clear_value = info.clear_color_values[clear_color_att.index].value;
        if(std::holds_alternative<ClearColorFloatValue>(clear_value))
        {
            parent->GetLoader().ClearNamedFramebufferfv(
                info.framebuffer->handle,
                GL_COLOR,
                clear_color_att.index,
                std::get<ClearColorFloatValue>(clear_value).data());
        }
        else if(std::holds_alternative<ClearColorIntValue>(clear_value))
        {
            parent->GetLoader().ClearNamedFramebufferiv(
                info.framebuffer->handle,
                GL_COLOR,
                clear_color_att.index,
                std::get<ClearColorIntValue>(clear_value).data());
        }
        else
        {
            parent->GetLoader().ClearNamedFramebufferuiv(
                info.framebuffer->handle,
                GL_COLOR,
                clear_color_att.index,
                std::get<ClearColorUIntValue>(clear_value).data());
        }
    }

    if(clear_depth_stencil_attachment_description.has_value())
    {
        parent->GetLoader().ClearNamedFramebufferfi(info.framebuffer->handle,
                                                    GL_DEPTH_STENCIL,
                                                    0,
                                                    info.clear_depth_stencil_value.depth,
                                                    info.clear_depth_stencil_value.stencil);
    }
}

void RenderPass::End()
{
    //noop
}

bool RenderPass::IsCreated() const noexcept
{
    return !clear_color_attachment_descriptions.empty() ||
           clear_depth_stencil_attachment_description.has_value();
}

void RenderPass::destroy() noexcept
{
    //noop
}
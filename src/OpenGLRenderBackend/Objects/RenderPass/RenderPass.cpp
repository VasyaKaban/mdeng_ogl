#include "RenderPass.h"
#include "../../Context/Context.h"
#include "../Framebuffer/Framebuffer.h"

namespace OpenGL
{
    RenderPass::RenderPass(Context* _parent, const Render::RenderPassInfo& info)
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
        //noop
    }

    void RenderPass::Begin(const Render::RenderPassBeginInfo& info)
    {
        parent->GetLoader().Disable(GL_SCISSOR_TEST); //explicitly disable scissors test

        //bind framebuffer
        GLHandle fb_handle = static_cast<const Framebuffer*>(info.framebuffer)->GetHandle();
        if(fb_handle != OGL_NULL_HANDLE)
            parent->GetLoader().BindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_handle);
        else
            parent->GetLoader().BindFramebuffer(
                GL_FRAMEBUFFER,
                fb_handle); //Bind default framebuffer for read and draw

        //clear attachments
        for(const auto& clear_color_att: clear_color_attachment_descriptions)
        {
            auto& clear_value = info.clear_color_values[clear_color_att.index].value;
            if(std::holds_alternative<Render::ClearColorFloatValue>(clear_value))
            {
                parent->GetLoader().ClearNamedFramebufferfv(
                    fb_handle,
                    GL_COLOR,
                    clear_color_att.index,
                    std::get<Render::ClearColorFloatValue>(clear_value).data());
            }
            else if(std::holds_alternative<Render::ClearColorIntValue>(clear_value))
            {
                parent->GetLoader().ClearNamedFramebufferiv(
                    fb_handle,
                    GL_COLOR,
                    clear_color_att.index,
                    std::get<Render::ClearColorIntValue>(clear_value).data());
            }
            else
            {
                parent->GetLoader().ClearNamedFramebufferuiv(
                    fb_handle,
                    GL_COLOR,
                    clear_color_att.index,
                    std::get<Render::ClearColorUIntValue>(clear_value).data());
            }
        }

        if(clear_depth_stencil_attachment_description.has_value())
        {
            parent->GetLoader().ClearNamedFramebufferfi(fb_handle,
                                                        GL_DEPTH_STENCIL,
                                                        0,
                                                        info.clear_depth_stencil_value.depth,
                                                        info.clear_depth_stencil_value.stencil);
        }
    }

    Render::Context* RenderPass::GetContext() const noexcept
    {
        return parent;
    }
};
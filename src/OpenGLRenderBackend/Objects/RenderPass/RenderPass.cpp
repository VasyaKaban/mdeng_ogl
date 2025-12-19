#include "RenderPass.h"
#include "../Device/Device.h"
#include "../Framebuffer/Framebuffer.h"
#include "Core/Render/Format.h"

namespace OpenGL
{
    RenderPass::RenderPass(Device* _parent, const Render::RenderPassInfo& info)
        : parent(_parent)
    {
        color_attachment_descriptions.reserve(info.color_attachments.size());
        for(std::size_t i = 0; i < info.color_attachments.size(); i++)
        {
            const auto& att = info.color_attachments[i];
            color_attachment_descriptions.push_back(ColorAttachmentDescription{
                .type =
                    Render::GetFormatType(att.format, Render::ImageAspectFlagBits::AspectColorBit),
                .clear = (att.load_op == Render::AttachmentLoadOp::Clear)});
        }

        if(info.depth_stencil_attachment)
        {
            depth_stencil_attachment_description = DepthStencilAttachmentDescription{
                .clear_depth =
                    (info.depth_stencil_attachment->load_op == Render::AttachmentLoadOp::Clear),
                .clear_stencil = (info.depth_stencil_attachment->stencil_load_op ==
                                  Render::AttachmentLoadOp::Clear)};
        }
    }

    RenderPass::~RenderPass()
    {
        //noop
    }

    Render::Device* RenderPass::GetParent() const noexcept
    {
        return parent;
    }

    void RenderPass::Begin(CommandBuffer& cmd, const Render::RenderPassBeginInfo& info)
    {
        parent->GetLoader().Enable(GL_SCISSOR_TEST);
        parent->GetLoader().Scissor(info.render_area.offset.x,
                                    info.render_area.offset.y,
                                    info.render_area.extent.width,
                                    info.render_area.extent.height);

        //bind framebuffer
        GLHandle fb_handle = static_cast<const Framebuffer*>(info.framebuffer)->GetHandle();
        //if(fb_handle != OGL_NULL_HANDLE)
        parent->GetLoader().BindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_handle);
        //else
        //    parent->GetLoader().BindFramebuffer(
        //        GL_FRAMEBUFFER,
        //        fb_handle); //Bind default framebuffer for read and draw

        for(std::size_t i = 0; i < info.clear_color_values.size(); i++)
        {
            if(!color_attachment_descriptions[i].clear)
                continue;

            switch(color_attachment_descriptions[i].type)
            {
                case Render::FormatType::UFLOAT:
                case Render::FormatType::SFLOAT:
                case Render::FormatType::UNORM:
                case Render::FormatType::SNORM:
                    parent->GetLoader().ClearNamedFramebufferfv(fb_handle,
                                                                GL_COLOR,
                                                                GL_DRAW_BUFFER0 + i,
                                                                info.clear_color_values[i].float32);
                    break;
                case Render::FormatType::UINT:
                    parent->GetLoader().ClearNamedFramebufferuiv(fb_handle,
                                                                 GL_COLOR,
                                                                 GL_DRAW_BUFFER0 + i,
                                                                 info.clear_color_values[i].uint32);
                    break;
                case Render::FormatType::SINT:
                    parent->GetLoader().ClearNamedFramebufferiv(fb_handle,
                                                                GL_COLOR,
                                                                GL_DRAW_BUFFER0 + i,
                                                                info.clear_color_values[i].int32);
                    break;
                default:
                    break;
            }
        }

        if(depth_stencil_attachment_description)
        {
            if(depth_stencil_attachment_description->clear_depth &&
               depth_stencil_attachment_description->clear_stencil)
            {
                parent->GetLoader().ClearNamedFramebufferfi(fb_handle,
                                                            GL_DEPTH_STENCIL,
                                                            0,
                                                            info.clear_depth_stencil_value.depth,
                                                            info.clear_depth_stencil_value.stencil);
            }
            else if(depth_stencil_attachment_description->clear_depth)
            {
                parent->GetLoader().ClearNamedFramebufferfv(fb_handle,
                                                            GL_DEPTH,
                                                            0,
                                                            &info.clear_depth_stencil_value.depth);
            }
            else if(depth_stencil_attachment_description->clear_stencil)
            {
                parent->GetLoader().ClearNamedFramebufferuiv(
                    fb_handle,
                    GL_STENCIL,
                    0,
                    &info.clear_depth_stencil_value.stencil);
            }
        }
    }
};
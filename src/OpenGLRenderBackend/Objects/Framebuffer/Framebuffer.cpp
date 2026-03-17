#include "Framebuffer.h"
#include "../Device/Device.h"
#include <stdexcept>
#include "../Image/Image.h"
#include "../ImageView/ImageView.h"
#include "Core/Utils/ScopedCall.hpp"

namespace OpenGL
{
    Framebuffer::Framebuffer(Device* _parent)
        : parent(_parent),
          handle(OGL_NULL_HANDLE)
    {}
    Framebuffer::Framebuffer(Device* _parent, const Render::FramebufferInfo& info)
        : parent(_parent)
    {
        GLHandle _handle;
        parent->GetLoader().CreateFramebuffers(1, &_handle);
        if(_handle == OGL_NULL_HANDLE)
            throw std::runtime_error("Failed to create framebuffer");

        Core::ScopedCall cleanup(
            [&_handle, _parent]()
            {
                _parent->GetLoader().DeleteFramebuffers(1, &_handle);
            });

        for(std::size_t i = 0; i < info.color_attachments.size(); i++)
        {
            parent->GetLoader().NamedFramebufferTexture(
                _handle,
                GL_COLOR_ATTACHMENT0 + i,
                static_cast<const ImageView*>(info.color_attachments[i])->GetHandle(),
                0);
        }

        if(info.depth_stencil_attachment)
        {
            parent->GetLoader().NamedFramebufferTexture(
                _handle,
                GL_DEPTH_STENCIL_ATTACHMENT,
                static_cast<const ImageView*>(info.depth_stencil_attachment)->GetHandle(),
                0);
        }

        if(parent->GetLoader().CheckNamedFramebufferStatus(_handle, GL_FRAMEBUFFER) !=
           GL_FRAMEBUFFER_COMPLETE)
        {
            throw std::runtime_error("Failed to create framebuffer. Bad attachments");
        }

        cleanup.Drop();

        handle = _handle;
    }

    Framebuffer::~Framebuffer()
    {
        if(handle != OGL_NULL_HANDLE)
            parent->GetLoader().DeleteFramebuffers(1, &handle);
    }

    Render::Device* Framebuffer::GetParent() const noexcept
    {
        return parent;
    }

    GLHandle Framebuffer::GetHandle() const noexcept
    {
        return handle;
    }
};
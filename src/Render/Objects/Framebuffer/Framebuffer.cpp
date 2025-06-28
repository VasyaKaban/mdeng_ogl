#include "Framebuffer.h"
#include "../../Context/Context.h"
#include <stdexcept>
#include "../Image/Image.h"
#include "../ImageView/ImageView.h"
#include "../../../hrs/scoped_call.hpp"

Framebuffer::Framebuffer() noexcept
    : parent(nullptr),
      handle(GL_NULL_HANDLE)
{}

Framebuffer::Framebuffer(Context* _parent)
    : parent(_parent),
      handle(GL_NULL_HANDLE),
      is_default(true)
{}

Framebuffer::Framebuffer(Context* _parent, const FramebufferInfo& info)
    : parent(_parent)
{
    GLHandle _handle;
    parent->GetLoader().CreateFramebuffers(1, &_handle);
    if(_handle == GL_NULL_HANDLE)
        throw std::runtime_error("Failed to create framebuffer");

    hrs::scoped_call cleanup(
        [&_handle, _parent]()
        {
            _parent->GetLoader().DeleteFramebuffers(1, &_handle);
        });

    auto get_attachment_handle = [](const AttachmentRef& ref)
    {
        if(std::holds_alternative<const Image*>(ref.attachment))
            return std::get<const Image*>(ref.attachment)->handle;
        else
            return std::get<const ImageView*>(ref.attachment)->handle;
    };

    for(std::size_t i = 0; i < info.color_attachments.size(); i++)
    {
        parent->GetLoader().NamedFramebufferTexture(
            _handle,
            GL_COLOR_ATTACHMENT0 + i,
            get_attachment_handle(info.color_attachments[i]),
            0);
    }

    if(info.depth_stencil_attachment)
    {
        parent->GetLoader().NamedFramebufferTexture(
            _handle,
            GL_DEPTH_STENCIL_ATTACHMENT,
            get_attachment_handle(*info.depth_stencil_attachment),
            0);
    }

    if(parent->GetLoader().CheckNamedFramebufferStatus(_handle, GL_FRAMEBUFFER) !=
       GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error("Failed to create framebuffer. Bad attachments");
    }

    cleanup.drop();

    handle = _handle;
}

Framebuffer::~Framebuffer()
{
    destroy();
}

Framebuffer::Framebuffer(Framebuffer&& fb) noexcept
    : parent(fb.parent),
      handle(std::exchange(fb.handle, GL_NULL_HANDLE))
{}

Framebuffer& Framebuffer::operator=(Framebuffer&& fb) noexcept
{
    destroy();

    parent = fb.parent;
    handle = std::exchange(fb.handle, GL_NULL_HANDLE);

    return *this;
}

bool Framebuffer::IsCreated() const noexcept
{
    return handle != GL_NULL_HANDLE;
}

void Framebuffer::destroy() noexcept
{
    if(IsCreated() && !is_default)
        parent->GetLoader().DeleteFramebuffers(1, &handle);
}

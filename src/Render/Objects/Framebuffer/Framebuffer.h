#pragma once

#include <variant>
#include <span>
#include "../../Common.h"
#include "../../../hrs/non_creatable.hpp"

class Context;
class Image;
class ImageView;

struct AttachmentRef
{
    std::variant<const Image*, const ImageView*> attachment;
};

struct FramebufferInfo
{
    std::span<const AttachmentRef> color_attachments;
    const AttachmentRef* depth_stencil_attachment;
};

class Framebuffer : hrs::non_copyable
{
    friend class RenderPass;
    friend class Context;

    Framebuffer(Context* _parent);
public:
    Framebuffer() noexcept;
    Framebuffer(Context* _parent, const FramebufferInfo& info);
    ~Framebuffer();
    Framebuffer(Framebuffer&& fb) noexcept;
    Framebuffer& operator=(Framebuffer&& fb) noexcept;

    [[deprecated]] GLHandle GetHandle() const noexcept
    {
        return handle;
    }

    bool IsCreated() const noexcept;
private:
    void destroy() noexcept;
private:
    Context* parent;
    GLHandle handle;
    bool is_default;
};

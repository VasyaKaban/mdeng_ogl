#pragma once

#include <span>
#include <vector>
#include <variant>
#include <array>
#include <optional>
#include "../../Common.h"
#include "../../../hrs/non_creatable.hpp"

class Context;
class Framebuffer;

struct AttachmentDescription
{
    bool clear_load;
};

struct RenderPassInfo
{
    std::span<const AttachmentDescription> color_attachment_descriptions;
    const AttachmentDescription* depth_stencil_attachment_description;
};

using ClearColorFloatValue = std::array<float, 4>;
using ClearColorIntValue = std::array<std::int32_t, 4>;
using ClearColorUIntValue = std::array<std::uint32_t, 4>;

struct ClearColorValue
{
    std::variant<ClearColorFloatValue, ClearColorIntValue, ClearColorUIntValue> value;
};

struct ClearDepthStencilValue
{
    float depth;
    std::uint32_t stencil;
};

struct RenderPassBeginInfo
{
    Framebuffer* framebuffer;
    std::span<const ClearColorValue> clear_color_values;
    ClearDepthStencilValue clear_depth_stencil_value;
};

class RenderPass : hrs::non_copyable
{
public:
    RenderPass() noexcept;
    RenderPass(Context* _parent, const RenderPassInfo& info);
    ~RenderPass();
    RenderPass(RenderPass&& rpass) noexcept;
    RenderPass& operator=(RenderPass&& rpass) noexcept;

    void Begin(const RenderPassBeginInfo& info);

    void End();

    bool IsCreated() const noexcept;
private:
    void destroy() noexcept;
private:
    Context* parent;

    struct ClearAttachmentDescription
    {
        AttachmentDescription desc;
        GLuint index;
    };

    std::vector<ClearAttachmentDescription> clear_color_attachment_descriptions;
    std::optional<AttachmentDescription> clear_depth_stencil_attachment_description;
    //GLHandle handle; no handle at all!!!
};

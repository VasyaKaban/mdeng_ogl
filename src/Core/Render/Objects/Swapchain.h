#pragma once

#include <optional>
#include "../Render.h"

namespace Render
{
    class CORE_API Swapchain
    {
    public:
        virtual ~Swapchain() = 0;

        virtual std::vector<Image*> GetSwapchainImages() const = 0;
        virtual Framebuffer* CreateFramebufferFromSwapchainImage(std::uint32_t index,
                                                                 RenderPass* renderpass) = 0;

        virtual std::optional<std::uint32_t>
        AcquireNextSwapchainImage(const Render::AcquireNextImageInfo& info) = 0;

        virtual bool PresentSwapchainImage(const PresentInfo& info) = 0;

        virtual void Recreate(const SwapchainInfo& info) = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};
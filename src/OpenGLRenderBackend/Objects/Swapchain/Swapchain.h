#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Swapchain.h"

namespace OpenGL
{
    class Swapchain : public Render::Swapchain, hrs::non_copyable, hrs::non_movable
    {
    public:
        Swapchain(Device* _parent, Surface* _surface, const Render::SwapchainInfo& info);

        virtual ~Swapchain() override;

        virtual std::vector<Render::Image*> GetSwapchainImages() const override;
        virtual Render::Framebuffer*
        CreateFramebufferFromSwapchainImage(std::uint32_t index,
                                            Render::RenderPass* renderpass) override;

        virtual std::optional<std::uint32_t>
        AcquireNextSwapchainImage(Render::Semaphore* signal_semaphore) override;

        virtual bool PresentSwapchainImage(const Render::PresentInfo& info) override;

        virtual void Recreate(const Render::SwapchainInfo& info) override;

        virtual Render::Device* GetParent() const noexcept override;
    private:
        Device* parent;
        Surface* surface;
        std::array<Render::Image*, SWAPCHAIN_IMAGE_COUNT> images;

        std::uint32_t image_index;
    };
};
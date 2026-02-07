#include "Swapchain.h"
#include "../Surface/Surface.h"
#include "../Framebuffer/Framebuffer.h"
#include "../Semaphore/Semaphore.h"
#include "../Fence/Fence.h"
#include "../Device/Device.h"

namespace OpenGL
{
    Swapchain::Swapchain(Device* _parent, Surface* _surface, const Render::SwapchainInfo& info)
        : parent(_parent),
          surface(_surface),
          image_index(0)
    {
        for(std::uint32_t i = 0; i < SWAPCHAIN_IMAGE_COUNT; i++)
            images[i] = reinterpret_cast<Render::Image*>(
                this); //dirty pseudo-swapchain image -> not for use, just for pipeline barriers
    }

    Swapchain::~Swapchain()
    {}

    std::vector<Render::Image*> Swapchain::GetSwapchainImages() const
    {
        return std::vector<Render::Image*>{images.begin(), images.end()};
    }

    Render::Framebuffer*
    Swapchain::CreateFramebufferFromSwapchainImage(std::uint32_t index,
                                                   Render::RenderPass* renderpass)
    {
        return new Framebuffer(parent);
    }

    std::optional<std::uint32_t>
    Swapchain::AcquireNextSwapchainImage(const Render::AcquireNextImageInfo& info)
    {
        if(info.semaphore)
            static_cast<Semaphore*>(info.semaphore)->Set();

        if(info.fence)
            static_cast<Fence*>(info.fence)->Set();

        return image_index;
    }

    bool Swapchain::PresentSwapchainImage(const Render::PresentInfo& info)
    {
        surface->SwapWindow();

        image_index = (image_index + 1) % SWAPCHAIN_IMAGE_COUNT;

        return true;
    }

    void Swapchain::Recreate(const Render::SwapchainInfo& info)
    {
        surface->SetSwapInterval(info.present_mode);
    }

    Render::Device* Swapchain::GetParent() const noexcept
    {
        return parent;
    }
};
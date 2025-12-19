#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Framebuffer.h"

namespace OpenGL
{
    class Framebuffer : public Render::Framebuffer, hrs::non_copyable, hrs::non_movable
    {
    public:
        Framebuffer(Device* _parent);
        Framebuffer(Device* _parent, const Render::FramebufferInfo& info);
        virtual ~Framebuffer() override;

        virtual Render::Device* GetParent() const noexcept override;

        GLHandle GetHandle() const noexcept;
    private:
        Device* parent;
        GLHandle handle;
    };
};
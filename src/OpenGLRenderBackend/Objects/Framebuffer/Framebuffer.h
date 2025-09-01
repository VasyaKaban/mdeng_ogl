#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Framebuffer.h"

namespace OpenGL
{
    class Framebuffer : public Render::Framebuffer, hrs::non_copyable, hrs::non_movable
    {
    public:
        Framebuffer(Context* _parent);
        Framebuffer(Context* _parent, const Render::FramebufferInfo& info);
        virtual ~Framebuffer() override;

        GLHandle GetHandle() const noexcept;

        virtual Render::Context* GetContext() const noexcept override;
    private:
        Context* parent;
        GLHandle handle;
    };
};
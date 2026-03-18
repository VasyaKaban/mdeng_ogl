#pragma once

#include "Core/Utils/NonCreatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Framebuffer.h"

namespace OpenGL
{
    class Framebuffer final : public Render::Framebuffer, Core::NonCopyable, Core::NonMovable
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
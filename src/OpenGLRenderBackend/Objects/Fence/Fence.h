#pragma once

#include "Core/Utils/NonCreatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Fence.h"

namespace OpenGL
{
    class Fence final : public Render::Fence, Core::NonCopyable, Core::NonMovable
    {
    public:
        Fence(Device* _parent) noexcept;
        virtual ~Fence() override;

        void Set();
        virtual bool Wait(std::uint64_t timeout_ns) noexcept override;
        virtual bool Reset() noexcept override;
        virtual Render::FenceStatus GetStatus() const noexcept override;

        virtual Render::Device* GetParent() const noexcept override;

        GLsync GetHandle() const noexcept;
    private:
        Device* parent;
        GLsync handle;
    };
};
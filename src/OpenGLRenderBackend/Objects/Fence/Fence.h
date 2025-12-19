#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Fence.h"

namespace OpenGL
{
    class Fence : public Render::Fence, hrs::non_copyable, hrs::non_movable
    {
        friend class Queue;
        void Set();
    public:
        Fence(Device* _parent) noexcept;
        virtual ~Fence() override;

        virtual bool Wait(std::uint64_t timeout_ns) noexcept override;
        virtual Render::FenceStatus GetStatus() const noexcept override;

        virtual Render::Device* GetParent() const noexcept override;

        GLsync GetHandle() const noexcept;
    private:
        Device* parent;
        GLsync handle;
    };
};
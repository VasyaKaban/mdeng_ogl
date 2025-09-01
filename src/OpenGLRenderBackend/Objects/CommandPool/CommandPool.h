#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/CommandPool.h"

namespace OpenGL
{
    class CommandPool : public Render::CommandPool, hrs::non_copyable, hrs::non_movable
    {
    public:
        CommandPool(Context* _parent, const Render::CommandPoolInfo& info) noexcept;
        virtual ~CommandPool() override;

        virtual Render::CommandBuffer* Allocate() override;

        virtual Render::Context* GetContext() const noexcept override;
    private:
        Context* parent;
        const Queue* queue;
    };
};
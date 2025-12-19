#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/CommandPool.h"

namespace OpenGL
{
    class CommandPool : public Render::CommandPool, hrs::non_copyable, hrs::non_movable
    {
    public:
        CommandPool(Device* _parent, const Render::CommandPoolInfo& info) noexcept;
        virtual ~CommandPool() override;

        virtual Render::CommandBuffer* Allocate() override;

        virtual Render::Device* GetParent() const noexcept override;
    private:
        Device* parent;
    };
};
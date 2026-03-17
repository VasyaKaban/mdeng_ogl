#pragma once

#include "Core/Utils/NonCreatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/CommandPool.h"

namespace OpenGL
{
    class CommandPool : public Render::CommandPool, Core::NonCopyable, Core::NonMovable
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
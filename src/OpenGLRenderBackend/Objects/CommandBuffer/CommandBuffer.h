#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/CommandBuffer.h"

namespace OpenGL
{
    //imitation only!
    class CommandBuffer : public Render::CommandBuffer, hrs::non_copyable, hrs::non_movable
    {
    public:
        CommandBuffer(Context* _parent, CommandPool* _pool) noexcept;
        virtual ~CommandBuffer() override;

        virtual void Reset() override;
        virtual void Begin() override;
        virtual void End() override;

        virtual Render::Context* GetContext() const noexcept override;

        //Commands:
        //void SetMemoryBarrier(MemoryBarrierFlags flags, bool by_region) const noexcept;
    private:
        Context* parent;
        CommandPool* pool;
    };
};
#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Queue.h"

namespace OpenGL
{
    class Queue : public Render::Queue, hrs::non_copyable, hrs::non_movable
    {
    public:
        Queue(Context* _parent) noexcept;

        virtual ~Queue() override;

        virtual void Begin(const Render::QueueBeginInfo& info) override;
        virtual void Flush(const Render::QueueFlushInfo& info) override;

        virtual void WaitIdle() override;

        virtual Render::Context* GetContext() const noexcept override;
    private:
        Context* parent;
    };
};
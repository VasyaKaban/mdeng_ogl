#pragma once

#include "Core/Utils/NonCreatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Queue.h"

namespace OpenGL
{
    class Queue final : public Render::Queue, Core::NonCopyable, Core::NonMovable
    {
    public:
        Queue(Device* _parent) noexcept;

        virtual ~Queue() override;

        virtual void Begin(const Render::QueueBeginInfo& info) override;
        virtual void Flush(const Render::QueueFlushInfo& info) override;

        virtual void WaitIdle() override;

        virtual Render::Device* GetParent() const noexcept override;
    private:
        Device* parent;
    };
};
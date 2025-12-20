#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Semaphore.h"

#define OPENGL_NOOP_SEMAPHORE

namespace OpenGL
{
    class Semaphore : public Render::Semaphore, hrs::non_copyable, hrs::non_movable
    {
        friend class Swapchain;
        friend class Queue;
        void Wait();
        void Set();
    public:
        Semaphore(Device* _parent);
        virtual ~Semaphore() override;

        virtual Render::Device* GetParent() const noexcept override;

        GLsync GetHandle() const noexcept;
    private:
        Device* parent;

#ifndef OPENGL_NOOP_SEMAPHORE
        GLsync handle;
#endif
    };
};
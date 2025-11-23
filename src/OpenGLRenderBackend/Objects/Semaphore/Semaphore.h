#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Semaphore.h"

#define OPENGL_NOOP_SEMAPHORE

namespace OpenGL
{
    class Semaphore : public Render::Semaphore, hrs::non_copyable, hrs::non_movable
    {
        friend class Context;
        friend class Queue;
        void Wait();
        void Set();
    public:
        Semaphore(Context* _parent);
        virtual ~Semaphore() override;

        GLsync GetHandle() const noexcept;

        virtual Render::Context* GetContext() const noexcept override;
    private:
        Context* parent;

#ifndef OPENGL_NOOP_SEMAPHORE
        GLsync handle;
#endif
    };
};
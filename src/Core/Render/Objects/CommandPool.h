#pragma once

#include <memory>
#include "../Render.h"
#include "Object.h"
#include "CommandBuffer.h"

namespace Render
{
    class CommandPool : public Object
    {
    public:
        virtual ~CommandPool()
        {}

        virtual CommandBuffer* Allocate() = 0;

        std::unique_ptr<CommandBuffer> AllocateUnique();
    };
};
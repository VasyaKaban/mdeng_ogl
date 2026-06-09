#pragma once

#include <cstdint>
#include "Core/API.h"

namespace Core
{
    using InterfaceVersion = uint32_t;

    class CORE_API Interface
    {
    public:
        virtual ~Interface() = 0;

        virtual InterfaceVersion GetVersion() const noexcept = 0;
    };
};
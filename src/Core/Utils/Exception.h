#pragma once

#include "Types.hpp"
#include "../API.h"

namespace Core
{
    class StringView;

    class CORE_API Exception
    {
    public:
        virtual ~Exception() = 0;

        virtual StringView GetMessage() const noexcept = 0;
    };
};
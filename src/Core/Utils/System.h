#pragma once

#ifdef _WIN32
#    include "WinAPI/SystemBase.h"
#elif defined(linux)
#    include "Linux/SystemBase.h"
#else
#    error "Not implemented yet.."
#endif

namespace Core
{
    class CORE_API System : public SystemBase
    {};
};
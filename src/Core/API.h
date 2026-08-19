#pragma once

#include "Utils/Platform.h"

#if CORE_PLATFORM_CURRENT == CORE_PLATFORM_WIN32
#    ifdef CORE_API_EXPORT
#        define CORE_API __declspec(dllexport)
#    else
#        define CORE_API __declspec(dllimport)
#    endif
#    define CORE_API_TEMPLATE
#else
#    define CORE_API
#    ifdef CORE_API_EXPORT
#        define CORE_API_TEMPLATE
#    else
#        define CORE_API_TEMPLATE extern
#    endif
#endif
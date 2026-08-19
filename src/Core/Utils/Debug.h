#pragma once

#include "BuiltIn.h"

#ifdef CORE_DEBUG
#    define CORE_DEBUG_ASSERTION(COND, ...) \
        if(!(COND __VA_OPT__(, __VA_ARGS__))) \
        { \
            CORE_BREAKPOINT() \
        }
#else
#    define CORE_DEBUG_ASSERTION(COND, ...) CORE_ASSUME(COND __VA_OPT__(, __VA_ARGS__))
#endif
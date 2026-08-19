#pragma once

#if CORE_PLATFORM_CURRENT != CORE_PLATFORM_WIN32
#    include <signal.h>
#endif

#if CORE_COMPILER_CURRENT == CORE_COMPILER_MSVC
#    define CORE_ASSUME(COND, ...) __assume(COND __VA_OPT__(, __VA_ARGS__));

#    define CORE_UNREACHABLE() __assume(0);

#    define CORE_BREAKPOINT() __debugbreak();

#else
#    define CORE_ASSUME(COND, ...) [[gnu::assume(COND __VA_OPT__(, __VA_ARGS__))]];

#    define CORE_UNREACHABLE() __builtin_unreachable();

#    if CORE_COMPILER_CURRENT == CORE_COMPILER_CLANG
#        define CORE_BREAKPOINT() __builtin_debugtrap();
#    else
#        define CORE_BREAKPOINT() raise(SIGTRAP);
#    endif
#endif

#define CORE_ASSUME_ALIGNED(PTR, ALIGNMENT, ...) CORE_ASSUME(reinterpret_cast<DeviceSize>(PTR) % (ALIGNMENT __VA_OPT__(, __VA_ARGS__)) == 0)
#pragma once

//ARCH
#define CORE_ARCH_AMD64 1

#ifdef _M_AMD64
#    define CORE_ARCH_CURRENT CORE_ARCH_AMD64
#else
#    error "Unsupported architecture!"
#endif

//PLATFORM
#define CORE_PLATFORM_WIN32 1
#define CORE_PLATFORM_LINUX 2

#ifdef _WIN32
#    define CORE_PLATFORM_CURRENT CORE_PLATFORM_WIN32
#elif defined(__linux__)
#    define CORE_PLATFORM_CURRENT CORE_PLATFORM_LINUX
#else
#    error "Unsupported platform!"
#endif

//COMPILER
#define CORE_COMPILER_MSVC 1
#define CORE_COMPILER_CLANG 2
#define CORE_COMPILER_GCC 3

#ifdef _MSC_VER
#    define CORE_COMPILER_CURRENT CORE_COMPILER_MSVC
#elif defined(__clang__)
#    define CORE_COMPILER_CURRENT CORE_COMPILER_CLANG
#elif defined(__GNUC__)
#    define CORE_COMPILER_CURRENT CORE_COMPILER_GCC
#else
#    error "Unsupported compiler!"
#endif
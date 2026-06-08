#pragma once

#ifdef _WIN32
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

#define CORE_THROW_EXCEPTION_MOCK(ARG, ...) throw ARG;
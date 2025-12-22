#pragma once

#define UNICODE
#define _UNICODE
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include <Windows.h>
#include <stdexcept>

#undef CreateWindow
#undef CreateSemaphore
#undef MemoryBarrier
#undef GetMessage

namespace hrs
{
    std::runtime_error winapi_get_last_error();
};
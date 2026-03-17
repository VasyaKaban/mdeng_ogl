#pragma once

#include <optional>
#include <stdexcept>
#include "Core/Utils/Expected.hpp"
#include "Core/Utils/System.h"

namespace OpenGL
{
    std::optional<std::runtime_error> SetDefaultPixelFormat(HDC dc) noexcept;

    Core::Expected<HGLRC, std::runtime_error>
    CreateContext(HDC dc, bool debug_messenger_enabled, bool robust_buffer_access_enabled) noexcept;
};
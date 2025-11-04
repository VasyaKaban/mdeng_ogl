#pragma once

#ifdef _WIN32
#    include "detail/winapi/os.h"
#elif defined(linux)
#    include "detail/linux/os.h"
#else
#    error "Not implemented yet.."
#endif
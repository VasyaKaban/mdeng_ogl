#include "winapi.h"

namespace hrs
{
    std::runtime_error winapi_get_last_error()
    {
        DWORD error = GetLastError();
        char* buffer = nullptr;
        auto size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                       FORMAT_MESSAGE_IGNORE_INSERTS,
                                   nullptr,
                                   error,
                                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                   reinterpret_cast<LPSTR>(&buffer),
                                   0,
                                   nullptr);

        std::string message(buffer, size);

        LocalFree(buffer);

        return std::runtime_error(message);
    }

};
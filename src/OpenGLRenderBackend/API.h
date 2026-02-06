#pragma once

#ifdef _WIN32
#    define OGL_RENDER_BACKEND_API_EXPORT __declspec(dllexport)
#else
#    define OGL_RENDER_BACKEND_API_EXPORT
#endif
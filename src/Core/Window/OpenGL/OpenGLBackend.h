#pragma once

#include <SDL.h>
#include "hrs/non_creatable.hpp"
#include "../RenderBackend.h"
#include "../../Render/Render.h"

namespace Core
{
    class GraphicWindow;

    enum class WindowPresentMode
    {
        Immediate = 0,
        VSync = 1,
        AdaptiveVSync = -1
    };

    enum class OpenGLVersion;

#define MAKE_OPENGL_VERSION(MAJOR, MINOR, CORE) \
    MAJOR * 100 + MINOR * 10 + static_cast<int>(CORE == true)

    enum class OpenGLVersion
    {
        OpenGL_4_6_Core = MAKE_OPENGL_VERSION(4, 6, true)
    };

#undef MAKE_OPENGL_VERSION

    constexpr inline std::uint16_t GetOpenGLMajorVersion(OpenGLVersion version) noexcept
    {
        return static_cast<std::uint16_t>(version) / 100;
    }

    constexpr inline std::uint16_t GetOpenGLMinorVersion(OpenGLVersion version) noexcept
    {
        return (static_cast<std::uint16_t>(version) / 10) % 10;
    }

    constexpr inline bool IsOpenGLVersionCoreprofile(OpenGLVersion version) noexcept
    {
        return static_cast<std::uint16_t>(version) % 10;
    }

    struct OpenGLBackendDefaultFramebufferInfo
    {
        Render::Format format;
    };

    enum OpenGLBackendFlagBits
    {
        DebugContext = 1 << 0
    };

    using OpenGLBackendFlags = std::underlying_type_t<OpenGLBackendFlagBits>;

    struct OpenGLBackendInfo : public RenderBackendInfo
    {
        OpenGLBackendDefaultFramebufferInfo default_framebuffer_info;
        OpenGLVersion version;
        OpenGLBackendFlags flags;
    };

    using OpenGLGetProcAddr_PFN = void* (*)(const char* name);

    class OpenGLBackend : public RenderBackend, hrs::non_copyable, hrs::non_movable
    {
        friend class GraphicWindow;
        OpenGLBackend(GraphicWindow* _parent, const OpenGLBackendInfo& info);

        static void SetAttributes(const OpenGLBackendInfo& info);
    public:
        ~OpenGLBackend();

        virtual RenderBackendType GetType() const noexcept override;

        void SetPresentMode(WindowPresentMode mode);
        void MakeContextCurrent() noexcept;
        void AcquireNextSwapchainImage(); //OGL -> noop
        void ReleaseSwapchainImage(); //SDL_SwapWindow();

        OpenGLGetProcAddr_PFN GetResolver() const noexcept;
        OpenGLVersion GetVersion() const noexcept;
    private:
        SDL_GLContext handle;
        OpenGLVersion version;
    };

    static_assert(!std::is_abstract_v<OpenGLBackend>);
};
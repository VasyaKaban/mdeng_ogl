#pragma once

#include <ShellScalingApi.h>
#include "Core/Utils/DynamicLibrary.h"
#include "../WindowSubsystem.h"

namespace Core
{
    namespace Win32
    {
        class CORE_API WindowSubsystem : public Core::WindowSubsystem,
                                         Core::NonCopyable,
                                         Core::NonMovable
        {
        public:
            WindowSubsystem();

            virtual ~WindowSubsystem() override;

            virtual void PollEvents() override;

            virtual WindowSubsystemType GetType() const noexcept override;

            virtual Core::Window* CreateWindow(const WindowInfo& info) override;

            HINSTANCE GetInstance() const noexcept;
        private:
            HINSTANCE instance;

            DynamicLibrary user32;
            DynamicLibrary shcore;

            BOOL (*SetProcessDpiAwarenessContext)([in] DPI_AWARENESS_CONTEXT value);
            HRESULT (*SetProcessDpiAwareness)([in] PROCESS_DPI_AWARENESS value);
            BOOL (*SetProcessDPIAware)();
            DPI_AWARENESS_CONTEXT dpi_awareness;
        };
    };
};
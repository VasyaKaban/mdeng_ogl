#pragma once

#include <ShellScalingApi.h>
#include "Core/Utils/DynamicLibrary.h"
#include "../WindowSubsystem.h"

namespace Core
{
    namespace Win32
    {
        class CORE_API WindowSubsystem final : public Core::WindowSubsystem,
                                               Core::NonCopyable,
                                               Core::NonMovable
        {
        public:
            WindowSubsystem();

            virtual ~WindowSubsystem() override;

            virtual void PollEvents() override;

            virtual WindowSubsystemType GetType() const noexcept override;

            virtual Core::Window* CreateWindow(const WindowInfo& info) override;

            virtual CursorState GetCursorState() const override;
            virtual void SetCursorState(CursorState state) override;

            HINSTANCE GetInstance() const noexcept;

            PROCESS_DPI_AWARENESS GetDPIAwrenessType() const noexcept;
        private:
            HINSTANCE instance;

            DynamicLibrary user32;
            DynamicLibrary shcore;

            HRESULT (*SetProcessDpiAwareness)([in] PROCESS_DPI_AWARENESS value);
            BOOL (*SetProcessDPIAware)();
            PROCESS_DPI_AWARENESS dpi_awareness;
        public:
            //let's make it public
            const HRESULT (*GetDpiForMonitor)([in] HMONITOR hmonitor,
                                              [in] MONITOR_DPI_TYPE dpiType,
                                              [out] UINT* dpiX,
                                              [out] UINT* dpiY);
        };
    };
};
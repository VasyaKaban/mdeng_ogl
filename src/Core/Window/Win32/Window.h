#pragma once

#include "../Window.h"
#include "Display.h"

namespace Core
{
    namespace Win32
    {
        class WindowSubsystem;

        static thread_local std::exception_ptr WND_PROC_EXCEPTION = {};

        class CORE_API Window final : public Core::Window, Core::NonMovable
        {
        public:
            constexpr static wchar_t WIN32_WINDOW_CLASS_NAME[] = L"WIN32_WINDOW_CLASS";

            static LRESULT CALLBACK Win32WindowProc(HWND handle,
                                                    UINT message,
                                                    WPARAM w_param,
                                                    LPARAM l_param);

            Window(WindowSubsystem* _parent, const WindowInfo& info);

            virtual ~Window() override;

            virtual void SetTitle(std::string_view title) override;
            virtual std::string GetTitle() const override;

            virtual void Resize(const WindowResolution& resolution) override;
            virtual WindowResolution GetResolution() const override;
            virtual WindowResolution GetScaledResolution() const override;

            virtual void SetState(WindowState state) override;
            virtual WindowState GetState() const override;

            virtual void SetMouseCursorPosition(const WindowPosition& pos) override;
            virtual WindowPosition GetMouseCursorPosition() const override;

            virtual WindowSurfaceInfo GetWindowSurfaceInfo() const noexcept override;

            virtual Display* GetDisplay() const noexcept override;
            virtual Core::WindowSubsystem* GetParent() const noexcept override;
        private:
            void UpdatePrevWindowedState();
        private:
            WindowSubsystem* parent;
            HWND handle;
            std::unique_ptr<Display> display;
            WindowState current_state;
            WindowPosition windowed_prev_position;
            WindowResolution windowed_prev_resolution;
            bool mouse_focused;
        };
    };
};
#pragma once

#include "../Window.h"
#include "Display.h"

namespace Core
{
    namespace Win32
    {
        class WindowSubsystem;

        class CORE_API Window final : public Core::Window, Core::NonMovable
        {
        public:
            using EventEmitter::EmitRaw;

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

            virtual void SetVisibility(WindowVisibility visibility) override;
            virtual WindowVisibility GetVisibility() const override;

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
            WindowVisibility current_visibility;
            RECT prev_windowed_rect;
            bool mouse_focused;

            wchar_t high_surrogate;
        };
    };
};
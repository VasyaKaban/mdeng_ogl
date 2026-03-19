#pragma once

#include "../Window.h"

namespace Core
{
    namespace Win32
    {
        class WindowSubsystem;

        constexpr inline wchar_t WIN32_WINDOW_CLASS_NAME[] = L"WIN32_WINDOW_CLASS";

        LRESULT CALLBACK CORE_API Win32WindowProc(HWND handle,
                                                  UINT message,
                                                  WPARAM w_param,
                                                  LPARAM l_param);

        class CORE_API Window : public Core::Window, Core::NonMovable
        {
        public:
            Window(WindowSubsystem* _parent, const WindowInfo& info);

            virtual ~Window() override;

            virtual void SetTitle(std::string_view title) override;
            virtual std::string GetTitle() const override;

            virtual void Resize(const WindowResolution& resolution) override;
            virtual WindowResolution GetResolution() const override;
            virtual WindowResolution GetScaledResolution() const override;

            virtual float GetScaleFactor() const override; // return dpi / default_dpi;
            virtual float
            GetSurfaceScaleFactor() const override; // return scaled_resolution / resolution;

            virtual void SetState(WindowState state) override;
            virtual WindowState GetState() const override;

            virtual void SetMouseCursorPosition(const WindowPosition& pos) override;
            virtual WindowPosition GetMouseCursorPosition() const override;

            virtual WindowSurfaceInfo GetWindowSurfaceInfo() const noexcept override;

            virtual Core::WindowSubsystem* GetParent() const noexcept override;
        private:
            WindowSubsystem* parent;
            HWND handle;
        };
    };
};
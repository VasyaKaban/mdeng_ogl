#pragma once

#include <queue>
#include "Core/Utils/DynamicLibrary.h"
#include "Win32WindowSubsystem.h"
#include "../WindowEvents.h"
#include "../WindowSubsystemConnection.h"
#include "Win32Clipboard.h"
#include <dwmapi.h>

namespace Core
{
    struct Event
    {
        QueueEvent data;
        Win32Window* window;
        RCPointer<Win32Display> display;
    };

    struct DisplayNode
    {
        RCPointer<Display> display;
        bool active; //cached field to mark active and removed displays
    };

    struct User32Loader
    {
        constexpr static auto LIBRARY_NAME = "User32.dll";
        Core::DynamicLibrary lib;

        //Windows 7
        LONG (*GetDisplayConfigBufferSizes)(
            UINT32 flags,
            UINT32* numPathArrayElements,
            UINT32* numModeInfoArrayElements); //also in Vista but do not care

        //Windows 7
        LONG (*QueryDisplayConfig)(UINT32 flags,
                                   UINT32* numPathArrayElements,
                                   DISPLAYCONFIG_PATH_INFO* pathArray,
                                   UINT32* numModeInfoArrayElements,
                                   DISPLAYCONFIG_MODE_INFO* modeInfoArray,
                                   DISPLAYCONFIG_TOPOLOGY_ID* currentTopologyId);

        //Windows Vista
        LONG (*DisplayConfigGetDeviceInfo)(DISPLAYCONFIG_DEVICE_INFO_HEADER* requestPacket);

        //Windows 10 1607
        BOOL (*EnableNonClientDpiScaling)(HWND hwnd);

        //Windows 10 1703
        BOOL (*SetProcessDpiAwarenessContext)(DPI_AWARENESS_CONTEXT value);

        //Windows 10 1607
        DPI_AWARENESS_CONTEXT (*SetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT dpiContext);

        //Windows Vista
        BOOL (*SetProcessDPIAware)();
    };

    struct SHCoreLoader
    {
        constexpr static auto LIBRARY_NAME = "Shcore.dll";
        Core::DynamicLibrary lib;

        //Windows 8.1
        const HRESULT (*GetDpiForMonitor)(HMONITOR hmonitor,
                                          MONITOR_DPI_TYPE dpiType,
                                          UINT* dpiX,
                                          UINT* dpiY);

        //Windows 8.1
        HRESULT (*SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS value);
    };

    struct DWMAPILoader
    {
        constexpr static auto LIBRARY_NAME = "Dwmapi.dll";
        Core::DynamicLibrary lib;

        //Windows Vista
        HRESULT (*DwmEnableBlurBehindWindow)(HWND hWnd, const DWM_BLURBEHIND* pBlurBehind);
    };

    class CORE_API Win32WindowSubsystemConnection final : public Core::WindowSubsystemConnection
    {
    public:
        CORE_NON_COPYABLE(Win32WindowSubsystemConnection)
        CORE_NON_MOVABLE(Win32WindowSubsystemConnection)

        Win32WindowSubsystemConnection(const WindowSubsystemConnectionInfo& info);

        virtual ~Win32WindowSubsystemConnection() override;

        virtual void PollEvents() override;

        virtual WindowSubsystemConnectionType GetType() const noexcept override;

        virtual Window* CreateWindow(const WindowInfo& info) override;

        virtual CursorState GetCursorState() const override;
        virtual void SetCursorState(CursorState state) override;

        virtual KeyboardKey GetKeyByScancode(ScanCode scancode) override;
        virtual std::optional<ScanCode> GetScanCodeFromKey(KeyboardKey key) override;

        virtual KeyboardAccessState GetKeyboardAccessState() override;
        virtual void SetKeyboardAccessState(KeyboardAccessState state) override;

        virtual void GetClipboard(const std::function<ClipboardCallback>& callback) override;

        virtual std::vector<RCPointer<Display>> GetDisplays() override;

        const User32Loader& GetUser32Loader() const noexcept;
        const SHCoreLoader& GetSHCoreLoader() const noexcept;
        const DWMAPILoader& GetDWMAPILoader() const noexcept;

        Win32KeyboardState* GetKeyboardState() const noexcept;

        HINSTANCE GetInstance() const noexcept;
        DPI_AWARENESS_CONTEXT GetDPIAwrenessType() const noexcept;

        void PushEvent(Event&& event);

        void HandleDisplayChange(bool initial); //initial -> do not emit events
        RCPointer<Display> GetDisplayByMonitorHandle(HMONITOR handle) const noexcept;

        static Win32WindowSubsystemConnection* GetConnection() noexcept;

        void operator delete(void* ptr);
    private:
        static inline DPI_AWARENESS_CONTEXT DPI_AWARENESS = DPI_AWARENESS_CONTEXT_UNAWARE;
        static inline Win32WindowSubsystemConnection* SUBSYSTEM = nullptr;

        HINSTANCE instance;
        User32Loader user32;
        SHCoreLoader shcore;
        DWMAPILoader dwmapi;

        std::unique_ptr<Win32KeyboardState> keyboard_state;
        alignas(alignof(Clipboard)) std::byte clipboard[sizeof(Clipboard)];

        ATOM window_class_atom;

        std::unordered_map<HMONITOR, DisplayNode> displays;

        std::queue<Event> events;
    };
};
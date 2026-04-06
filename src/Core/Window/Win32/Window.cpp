#include "Window.h"
#include "WindowSubsystem.h"
#include "Core/Utils/ScopedCall.hpp"

#include <iostream>

namespace Core
{
    namespace Win32
    {
        struct WindowCreateData
        {
            Window* obj;
        };

        LRESULT CALLBACK Window::Win32WindowProc(HWND handle,
                                                 UINT message,
                                                 WPARAM w_param,
                                                 LPARAM l_param)
        {
            switch(message)
            {
                case WM_NULL:
                    std::cerr << "WM_NULL: " << WM_NULL << std::endl;
                    break;
                case WM_CREATE:
                    std::cerr << "WM_CREATE: " << WM_CREATE << std::endl;
                    break;
                case WM_DESTROY:
                    std::cerr << "WM_DESTROY: " << WM_DESTROY << std::endl;
                    break;
                case WM_MOVE:
                    std::cerr << "WM_MOVE: " << WM_MOVE << std::endl;
                    break;
                case WM_SIZE:
                    std::cerr << "WM_SIZE: " << WM_SIZE << std::endl;
                    break;
                case WM_ACTIVATE:
                    std::cerr << "WM_ACTIVATE: " << WM_ACTIVATE << std::endl;
                    break;
                case WM_SETFOCUS:
                    std::cerr << "WM_SETFOCUS: " << WM_SETFOCUS << std::endl;
                    break;
                case WM_KILLFOCUS:
                    std::cerr << "WM_KILLFOCUS: " << WM_KILLFOCUS << std::endl;
                    break;
                case WM_ENABLE:
                    std::cerr << "WM_ENABLE: " << WM_ENABLE << std::endl;
                    break;
                case WM_SETREDRAW:
                    std::cerr << "WM_SETREDRAW: " << WM_SETREDRAW << std::endl;
                    break;
                case WM_SETTEXT:
                    std::cerr << "WM_SETTEXT: " << WM_SETTEXT << std::endl;
                    break;
                case WM_GETTEXT:
                    std::cerr << "WM_GETTEXT: " << WM_GETTEXT << std::endl;
                    break;
                case WM_GETTEXTLENGTH:
                    std::cerr << "WM_GETTEXTLENGTH: " << WM_GETTEXTLENGTH << std::endl;
                    break;
                case WM_PAINT:
                    std::cerr << "WM_PAINT: " << WM_PAINT << std::endl;
                    break;
                case WM_CLOSE:
                    std::cerr << "WM_CLOSE: " << WM_CLOSE << std::endl;
                    break;
                case WM_QUERYENDSESSION:
                    std::cerr << "WM_QUERYENDSESSION: " << WM_QUERYENDSESSION << std::endl;
                    break;
                case WM_QUERYOPEN:
                    std::cerr << "WM_QUERYOPEN: " << WM_QUERYOPEN << std::endl;
                    break;
                case WM_ENDSESSION:
                    std::cerr << "WM_ENDSESSION: " << WM_ENDSESSION << std::endl;
                    break;
                case WM_QUIT:
                    std::cerr << "WM_QUIT: " << WM_QUIT << std::endl;
                    break;
                case WM_ERASEBKGND:
                    std::cerr << "WM_ERASEBKGND: " << WM_ERASEBKGND << std::endl;
                    break;
                case WM_SYSCOLORCHANGE:
                    std::cerr << "WM_SYSCOLORCHANGE: " << WM_SYSCOLORCHANGE << std::endl;
                    break;
                case WM_SHOWWINDOW:
                    std::cerr << "WM_SHOWWINDOW: " << WM_SHOWWINDOW << std::endl;
                    break;
                case WM_WININICHANGE:
                    std::cerr << "WM_WININICHANGE: " << WM_WININICHANGE << std::endl;
                    break;
                //case WM_SETTINGCHANGE:
                //    std::cerr << "WM_SETTINGCHANGE: " << WM_SETTINGCHANGE << std::endl;
                //    break;
                case WM_DEVMODECHANGE:
                    std::cerr << "WM_DEVMODECHANGE: " << WM_DEVMODECHANGE << std::endl;
                    break;
                case WM_ACTIVATEAPP:
                    std::cerr << "WM_ACTIVATEAPP: " << WM_ACTIVATEAPP << std::endl;
                    break;
                case WM_FONTCHANGE:
                    std::cerr << "WM_FONTCHANGE: " << WM_FONTCHANGE << std::endl;
                    break;
                case WM_TIMECHANGE:
                    std::cerr << "WM_TIMECHANGE: " << WM_TIMECHANGE << std::endl;
                    break;
                case WM_CANCELMODE:
                    std::cerr << "WM_CANCELMODE: " << WM_CANCELMODE << std::endl;
                    break;
                case WM_SETCURSOR:
                    std::cerr << "WM_SETCURSOR: " << WM_SETCURSOR << std::endl;
                    break;
                case WM_MOUSEACTIVATE:
                    std::cerr << "WM_MOUSEACTIVATE: " << WM_MOUSEACTIVATE << std::endl;
                    break;
                case WM_CHILDACTIVATE:
                    std::cerr << "WM_CHILDACTIVATE: " << WM_CHILDACTIVATE << std::endl;
                    break;
                case WM_QUEUESYNC:
                    std::cerr << "WM_QUEUESYNC: " << WM_QUEUESYNC << std::endl;
                    break;
                case WM_GETMINMAXINFO:
                    std::cerr << "WM_GETMINMAXINFO: " << WM_GETMINMAXINFO << std::endl;
                    break;
                case WM_PAINTICON:
                    std::cerr << "WM_PAINTICON: " << WM_PAINTICON << std::endl;
                    break;
                case WM_ICONERASEBKGND:
                    std::cerr << "WM_ICONERASEBKGND: " << WM_ICONERASEBKGND << std::endl;
                    break;
                case WM_NEXTDLGCTL:
                    std::cerr << "WM_NEXTDLGCTL: " << WM_NEXTDLGCTL << std::endl;
                    break;
                case WM_SPOOLERSTATUS:
                    std::cerr << "WM_SPOOLERSTATUS: " << WM_SPOOLERSTATUS << std::endl;
                    break;
                case WM_DRAWITEM:
                    std::cerr << "WM_DRAWITEM: " << WM_DRAWITEM << std::endl;
                    break;
                case WM_MEASUREITEM:
                    std::cerr << "WM_MEASUREITEM: " << WM_MEASUREITEM << std::endl;
                    break;
                case WM_DELETEITEM:
                    std::cerr << "WM_DELETEITEM: " << WM_DELETEITEM << std::endl;
                    break;
                case WM_VKEYTOITEM:
                    std::cerr << "WM_VKEYTOITEM: " << WM_VKEYTOITEM << std::endl;
                    break;
                case WM_CHARTOITEM:
                    std::cerr << "WM_CHARTOITEM: " << WM_CHARTOITEM << std::endl;
                    break;
                case WM_SETFONT:
                    std::cerr << "WM_SETFONT: " << WM_SETFONT << std::endl;
                    break;
                case WM_GETFONT:
                    std::cerr << "WM_GETFONT: " << WM_GETFONT << std::endl;
                    break;
                case WM_SETHOTKEY:
                    std::cerr << "WM_SETHOTKEY: " << WM_SETHOTKEY << std::endl;
                    break;
                case WM_GETHOTKEY:
                    std::cerr << "WM_GETHOTKEY: " << WM_GETHOTKEY << std::endl;
                    break;
                case WM_QUERYDRAGICON:
                    std::cerr << "WM_QUERYDRAGICON: " << WM_QUERYDRAGICON << std::endl;
                    break;
                case WM_COMPAREITEM:
                    std::cerr << "WM_COMPAREITEM: " << WM_COMPAREITEM << std::endl;
                    break;
                case WM_GETOBJECT:
                    std::cerr << "WM_GETOBJECT: " << WM_GETOBJECT << std::endl;
                    break;
                case WM_COMPACTING:
                    std::cerr << "WM_COMPACTING: " << WM_COMPACTING << std::endl;
                    break;
                case WM_COMMNOTIFY:
                    std::cerr << "WM_COMMNOTIFY: " << WM_COMMNOTIFY << std::endl;
                    break;
                case WM_WINDOWPOSCHANGING:
                    std::cerr << "WM_WINDOWPOSCHANGING: " << WM_WINDOWPOSCHANGING << std::endl;
                    break;
                case WM_WINDOWPOSCHANGED:
                    std::cerr << "WM_WINDOWPOSCHANGED: " << WM_WINDOWPOSCHANGED << std::endl;
                    break;
                case WM_POWER:
                    std::cerr << "WM_POWER: " << WM_POWER << std::endl;
                    break;
                case WM_COPYDATA:
                    std::cerr << "WM_COPYDATA: " << WM_COPYDATA << std::endl;
                    break;
                case WM_CANCELJOURNAL:
                    std::cerr << "WM_CANCELJOURNAL: " << WM_CANCELJOURNAL << std::endl;
                    break;
                case WM_NOTIFY:
                    std::cerr << "WM_NOTIFY: " << WM_NOTIFY << std::endl;
                    break;
                case WM_INPUTLANGCHANGEREQUEST:
                    std::cerr << "WM_INPUTLANGCHANGEREQUEST: " << WM_INPUTLANGCHANGEREQUEST
                              << std::endl;
                    break;
                case WM_INPUTLANGCHANGE:
                    std::cerr << "WM_INPUTLANGCHANGE: " << WM_INPUTLANGCHANGE << std::endl;
                    break;
                case WM_TCARD:
                    std::cerr << "WM_TCARD: " << WM_TCARD << std::endl;
                    break;
                case WM_HELP:
                    std::cerr << "WM_HELP: " << WM_HELP << std::endl;
                    break;
                case WM_USERCHANGED:
                    std::cerr << "WM_USERCHANGED: " << WM_USERCHANGED << std::endl;
                    break;
                case WM_NOTIFYFORMAT:
                    std::cerr << "WM_NOTIFYFORMAT: " << WM_NOTIFYFORMAT << std::endl;
                    break;
                case WM_CONTEXTMENU:
                    std::cerr << "WM_CONTEXTMENU: " << WM_CONTEXTMENU << std::endl;
                    break;
                case WM_STYLECHANGING:
                    std::cerr << "WM_STYLECHANGING: " << WM_STYLECHANGING << std::endl;
                    break;
                case WM_STYLECHANGED:
                    std::cerr << "WM_STYLECHANGED: " << WM_STYLECHANGED << std::endl;
                    break;
                case WM_DISPLAYCHANGE:
                    std::cerr << "WM_DISPLAYCHANGE: " << WM_DISPLAYCHANGE << std::endl;
                    break;
                case WM_GETICON:
                    std::cerr << "WM_GETICON: " << WM_GETICON << std::endl;
                    break;
                case WM_SETICON:
                    std::cerr << "WM_SETICON: " << WM_SETICON << std::endl;
                    break;
                case WM_NCCREATE:
                    std::cerr << "WM_NCCREATE: " << WM_NCCREATE << std::endl;
                    break;
                case WM_NCDESTROY:
                    std::cerr << "WM_NCDESTROY: " << WM_NCDESTROY << std::endl;
                    break;
                case WM_NCCALCSIZE:
                    std::cerr << "WM_NCCALCSIZE: " << WM_NCCALCSIZE << std::endl;
                    break;
                case WM_NCHITTEST:
                    std::cerr << "WM_NCHITTEST: " << WM_NCHITTEST << std::endl;
                    break;
                case WM_NCPAINT:
                    std::cerr << "WM_NCPAINT: " << WM_NCPAINT << std::endl;
                    break;
                case WM_NCACTIVATE:
                    std::cerr << "WM_NCACTIVATE: " << WM_NCACTIVATE << std::endl;
                    break;
                case WM_GETDLGCODE:
                    std::cerr << "WM_GETDLGCODE: " << WM_GETDLGCODE << std::endl;
                    break;
                case WM_SYNCPAINT:
                    std::cerr << "WM_SYNCPAINT: " << WM_SYNCPAINT << std::endl;
                    break;
                case WM_NCMOUSEMOVE:
                    std::cerr << "WM_NCMOUSEMOVE: " << WM_NCMOUSEMOVE << std::endl;
                    break;
                case WM_NCLBUTTONDOWN:
                    std::cerr << "WM_NCLBUTTONDOWN: " << WM_NCLBUTTONDOWN << std::endl;
                    break;
                case WM_NCLBUTTONUP:
                    std::cerr << "WM_NCLBUTTONUP: " << WM_NCLBUTTONUP << std::endl;
                    break;
                case WM_NCLBUTTONDBLCLK:
                    std::cerr << "WM_NCLBUTTONDBLCLK: " << WM_NCLBUTTONDBLCLK << std::endl;
                    break;
                case WM_NCRBUTTONDOWN:
                    std::cerr << "WM_NCRBUTTONDOWN: " << WM_NCRBUTTONDOWN << std::endl;
                    break;
                case WM_NCRBUTTONUP:
                    std::cerr << "WM_NCRBUTTONUP: " << WM_NCRBUTTONUP << std::endl;
                    break;
                case WM_NCRBUTTONDBLCLK:
                    std::cerr << "WM_NCRBUTTONDBLCLK: " << WM_NCRBUTTONDBLCLK << std::endl;
                    break;
                case WM_NCMBUTTONDOWN:
                    std::cerr << "WM_NCMBUTTONDOWN: " << WM_NCMBUTTONDOWN << std::endl;
                    break;
                case WM_NCMBUTTONUP:
                    std::cerr << "WM_NCMBUTTONUP: " << WM_NCMBUTTONUP << std::endl;
                    break;
                case WM_NCMBUTTONDBLCLK:
                    std::cerr << "WM_NCMBUTTONDBLCLK: " << WM_NCMBUTTONDBLCLK << std::endl;
                    break;
                case WM_NCXBUTTONDOWN:
                    std::cerr << "WM_NCXBUTTONDOWN: " << WM_NCXBUTTONDOWN << std::endl;
                    break;
                case WM_NCXBUTTONUP:
                    std::cerr << "WM_NCXBUTTONUP: " << WM_NCXBUTTONUP << std::endl;
                    break;
                case WM_NCXBUTTONDBLCLK:
                    std::cerr << "WM_NCXBUTTONDBLCLK: " << WM_NCXBUTTONDBLCLK << std::endl;
                    break;
                case WM_INPUT_DEVICE_CHANGE:
                    std::cerr << "WM_INPUT_DEVICE_CHANGE: " << WM_INPUT_DEVICE_CHANGE << std::endl;
                    break;
                case WM_INPUT:
                    std::cerr << "WM_INPUT: " << WM_INPUT << std::endl;
                    break;
                case WM_KEYDOWN:
                    std::cerr << "WM_KEYDOWN: " << WM_KEYDOWN << std::endl;
                    break;
                case WM_KEYUP:
                    std::cerr << "WM_KEYUP: " << WM_KEYUP << std::endl;
                    break;
                case WM_CHAR:
                    std::cerr << "WM_CHAR: " << WM_CHAR << std::endl;
                    break;
                case WM_DEADCHAR:
                    std::cerr << "WM_DEADCHAR: " << WM_DEADCHAR << std::endl;
                    break;
                case WM_SYSKEYDOWN:
                    std::cerr << "WM_SYSKEYDOWN: " << WM_SYSKEYDOWN << std::endl;
                    break;
                case WM_SYSKEYUP:
                    std::cerr << "WM_SYSKEYUP: " << WM_SYSKEYUP << std::endl;
                    break;
                case WM_SYSCHAR:
                    std::cerr << "WM_SYSCHAR: " << WM_SYSCHAR << std::endl;
                    break;
                case WM_SYSDEADCHAR:
                    std::cerr << "WM_SYSDEADCHAR: " << WM_SYSDEADCHAR << std::endl;
                    break;
                case WM_UNICHAR:
                    std::cerr << "WM_UNICHAR: " << WM_UNICHAR << std::endl;
                    break;
                case WM_IME_STARTCOMPOSITION:
                    std::cerr << "WM_IME_STARTCOMPOSITION: " << WM_IME_STARTCOMPOSITION
                              << std::endl;
                    break;
                case WM_IME_ENDCOMPOSITION:
                    std::cerr << "WM_IME_ENDCOMPOSITION: " << WM_IME_ENDCOMPOSITION << std::endl;
                    break;
                case WM_IME_COMPOSITION:
                    std::cerr << "WM_IME_COMPOSITION: " << WM_IME_COMPOSITION << std::endl;
                    break;
                case WM_INITDIALOG:
                    std::cerr << "WM_INITDIALOG: " << WM_INITDIALOG << std::endl;
                    break;
                case WM_COMMAND:
                    std::cerr << "WM_COMMAND: " << WM_COMMAND << std::endl;
                    break;
                case WM_SYSCOMMAND:
                    std::cerr << "WM_SYSCOMMAND: " << WM_SYSCOMMAND << std::endl;
                    break;
                case WM_TIMER:
                    std::cerr << "WM_TIMER: " << WM_TIMER << std::endl;
                    break;
                case WM_HSCROLL:
                    std::cerr << "WM_HSCROLL: " << WM_HSCROLL << std::endl;
                    break;
                case WM_VSCROLL:
                    std::cerr << "WM_VSCROLL: " << WM_VSCROLL << std::endl;
                    break;
                case WM_INITMENU:
                    std::cerr << "WM_INITMENU: " << WM_INITMENU << std::endl;
                    break;
                case WM_INITMENUPOPUP:
                    std::cerr << "WM_INITMENUPOPUP: " << WM_INITMENUPOPUP << std::endl;
                    break;
                case WM_GESTURE:
                    std::cerr << "WM_GESTURE: " << WM_GESTURE << std::endl;
                    break;
                case WM_GESTURENOTIFY:
                    std::cerr << "WM_GESTURENOTIFY: " << WM_GESTURENOTIFY << std::endl;
                    break;
                case WM_MENUSELECT:
                    std::cerr << "WM_MENUSELECT: " << WM_MENUSELECT << std::endl;
                    break;
                case WM_MENUCHAR:
                    std::cerr << "WM_MENUCHAR: " << WM_MENUCHAR << std::endl;
                    break;
                case WM_ENTERIDLE:
                    std::cerr << "WM_ENTERIDLE: " << WM_ENTERIDLE << std::endl;
                    break;
                case WM_MENURBUTTONUP:
                    std::cerr << "WM_MENURBUTTONUP: " << WM_MENURBUTTONUP << std::endl;
                    break;
                case WM_MENUDRAG:
                    std::cerr << "WM_MENUDRAG: " << WM_MENUDRAG << std::endl;
                    break;
                case WM_MENUGETOBJECT:
                    std::cerr << "WM_MENUGETOBJECT: " << WM_MENUGETOBJECT << std::endl;
                    break;
                case WM_UNINITMENUPOPUP:
                    std::cerr << "WM_UNINITMENUPOPUP: " << WM_UNINITMENUPOPUP << std::endl;
                    break;
                case WM_MENUCOMMAND:
                    std::cerr << "WM_MENUCOMMAND: " << WM_MENUCOMMAND << std::endl;
                    break;
                case WM_CHANGEUISTATE:
                    std::cerr << "WM_CHANGEUISTATE: " << WM_CHANGEUISTATE << std::endl;
                    break;
                case WM_UPDATEUISTATE:
                    std::cerr << "WM_UPDATEUISTATE: " << WM_UPDATEUISTATE << std::endl;
                    break;
                case WM_QUERYUISTATE:
                    std::cerr << "WM_QUERYUISTATE: " << WM_QUERYUISTATE << std::endl;
                    break;
                case WM_CTLCOLORMSGBOX:
                    std::cerr << "WM_CTLCOLORMSGBOX: " << WM_CTLCOLORMSGBOX << std::endl;
                    break;
                case WM_CTLCOLOREDIT:
                    std::cerr << "WM_CTLCOLOREDIT: " << WM_CTLCOLOREDIT << std::endl;
                    break;
                case WM_CTLCOLORLISTBOX:
                    std::cerr << "WM_CTLCOLORLISTBOX: " << WM_CTLCOLORLISTBOX << std::endl;
                    break;
                case WM_CTLCOLORBTN:
                    std::cerr << "WM_CTLCOLORBTN: " << WM_CTLCOLORBTN << std::endl;
                    break;
                case WM_CTLCOLORDLG:
                    std::cerr << "WM_CTLCOLORDLG: " << WM_CTLCOLORDLG << std::endl;
                    break;
                case WM_CTLCOLORSCROLLBAR:
                    std::cerr << "WM_CTLCOLORSCROLLBAR: " << WM_CTLCOLORSCROLLBAR << std::endl;
                    break;
                case WM_CTLCOLORSTATIC:
                    std::cerr << "WM_CTLCOLORSTATIC: " << WM_CTLCOLORSTATIC << std::endl;
                    break;
                case WM_MOUSEMOVE:
                    std::cerr << "WM_MOUSEMOVE: " << WM_MOUSEMOVE << std::endl;
                    break;
                case WM_LBUTTONDOWN:
                    std::cerr << "WM_LBUTTONDOWN: " << WM_LBUTTONDOWN << std::endl;
                    break;
                case WM_LBUTTONUP:
                    std::cerr << "WM_LBUTTONUP: " << WM_LBUTTONUP << std::endl;
                    break;
                case WM_LBUTTONDBLCLK:
                    std::cerr << "WM_LBUTTONDBLCLK: " << WM_LBUTTONDBLCLK << std::endl;
                    break;
                case WM_RBUTTONDOWN:
                    std::cerr << "WM_RBUTTONDOWN: " << WM_RBUTTONDOWN << std::endl;
                    break;
                case WM_RBUTTONUP:
                    std::cerr << "WM_RBUTTONUP: " << WM_RBUTTONUP << std::endl;
                    break;
                case WM_RBUTTONDBLCLK:
                    std::cerr << "WM_RBUTTONDBLCLK: " << WM_RBUTTONDBLCLK << std::endl;
                    break;
                case WM_MBUTTONDOWN:
                    std::cerr << "WM_MBUTTONDOWN: " << WM_MBUTTONDOWN << std::endl;
                    break;
                case WM_MBUTTONUP:
                    std::cerr << "WM_MBUTTONUP: " << WM_MBUTTONUP << std::endl;
                    break;
                case WM_MBUTTONDBLCLK:
                    std::cerr << "WM_MBUTTONDBLCLK: " << WM_MBUTTONDBLCLK << std::endl;
                    break;
                case WM_MOUSEWHEEL:
                    std::cerr << "WM_MOUSEWHEEL: " << WM_MOUSEWHEEL << std::endl;
                    break;
                case WM_XBUTTONDOWN:
                    std::cerr << "WM_XBUTTONDOWN: " << WM_XBUTTONDOWN << std::endl;
                    break;
                case WM_XBUTTONUP:
                    std::cerr << "WM_XBUTTONUP: " << WM_XBUTTONUP << std::endl;
                    break;
                case WM_XBUTTONDBLCLK:
                    std::cerr << "WM_XBUTTONDBLCLK: " << WM_XBUTTONDBLCLK << std::endl;
                    break;
                case WM_MOUSEHWHEEL:
                    std::cerr << "WM_MOUSEHWHEEL: " << WM_MOUSEHWHEEL << std::endl;
                    break;
                case WM_PARENTNOTIFY:
                    std::cerr << "WM_PARENTNOTIFY: " << WM_PARENTNOTIFY << std::endl;
                    break;
                case WM_ENTERMENULOOP:
                    std::cerr << "WM_ENTERMENULOOP: " << WM_ENTERMENULOOP << std::endl;
                    break;
                case WM_EXITMENULOOP:
                    std::cerr << "WM_EXITMENULOOP: " << WM_EXITMENULOOP << std::endl;
                    break;
                case WM_NEXTMENU:
                    std::cerr << "WM_NEXTMENU: " << WM_NEXTMENU << std::endl;
                    break;
                case WM_SIZING:
                    std::cerr << "WM_SIZING: " << WM_SIZING << std::endl;
                    break;
                case WM_CAPTURECHANGED:
                    std::cerr << "WM_CAPTURECHANGED: " << WM_CAPTURECHANGED << std::endl;
                    break;
                case WM_MOVING:
                    std::cerr << "WM_MOVING: " << WM_MOVING << std::endl;
                    break;
                case WM_POWERBROADCAST:
                    std::cerr << "WM_POWERBROADCAST: " << WM_POWERBROADCAST << std::endl;
                    break;
                case WM_DEVICECHANGE:
                    std::cerr << "WM_DEVICECHANGE: " << WM_DEVICECHANGE << std::endl;
                    break;
                case WM_MDICREATE:
                    std::cerr << "WM_MDICREATE: " << WM_MDICREATE << std::endl;
                    break;
                case WM_MDIDESTROY:
                    std::cerr << "WM_MDIDESTROY: " << WM_MDIDESTROY << std::endl;
                    break;
                case WM_MDIACTIVATE:
                    std::cerr << "WM_MDIACTIVATE: " << WM_MDIACTIVATE << std::endl;
                    break;
                case WM_MDIRESTORE:
                    std::cerr << "WM_MDIRESTORE: " << WM_MDIRESTORE << std::endl;
                    break;
                case WM_MDINEXT:
                    std::cerr << "WM_MDINEXT: " << WM_MDINEXT << std::endl;
                    break;
                case WM_MDIMAXIMIZE:
                    std::cerr << "WM_MDIMAXIMIZE: " << WM_MDIMAXIMIZE << std::endl;
                    break;
                case WM_MDITILE:
                    std::cerr << "WM_MDITILE: " << WM_MDITILE << std::endl;
                    break;
                case WM_MDICASCADE:
                    std::cerr << "WM_MDICASCADE: " << WM_MDICASCADE << std::endl;
                    break;
                case WM_MDIICONARRANGE:
                    std::cerr << "WM_MDIICONARRANGE: " << WM_MDIICONARRANGE << std::endl;
                    break;
                case WM_MDIGETACTIVE:
                    std::cerr << "WM_MDIGETACTIVE: " << WM_MDIGETACTIVE << std::endl;
                    break;
                case WM_MDISETMENU:
                    std::cerr << "WM_MDISETMENU: " << WM_MDISETMENU << std::endl;
                    break;
                case WM_ENTERSIZEMOVE:
                    std::cerr << "WM_ENTERSIZEMOVE: " << WM_ENTERSIZEMOVE << std::endl;
                    break;
                case WM_EXITSIZEMOVE:
                    std::cerr << "WM_EXITSIZEMOVE: " << WM_EXITSIZEMOVE << std::endl;
                    break;
                case WM_DROPFILES:
                    std::cerr << "WM_DROPFILES: " << WM_DROPFILES << std::endl;
                    break;
                case WM_MDIREFRESHMENU:
                    std::cerr << "WM_MDIREFRESHMENU: " << WM_MDIREFRESHMENU << std::endl;
                    break;
                case WM_POINTERDEVICECHANGE:
                    std::cerr << "WM_POINTERDEVICECHANGE: " << WM_POINTERDEVICECHANGE << std::endl;
                    break;
                case WM_POINTERDEVICEINRANGE:
                    std::cerr << "WM_POINTERDEVICEINRANGE: " << WM_POINTERDEVICEINRANGE
                              << std::endl;
                    break;
                case WM_POINTERDEVICEOUTOFRANGE:
                    std::cerr << "WM_POINTERDEVICEOUTOFRANGE: " << WM_POINTERDEVICEOUTOFRANGE
                              << std::endl;
                    break;
                case WM_TOUCH:
                    std::cerr << "WM_TOUCH: " << WM_TOUCH << std::endl;
                    break;
                case WM_NCPOINTERUPDATE:
                    std::cerr << "WM_NCPOINTERUPDATE: " << WM_NCPOINTERUPDATE << std::endl;
                    break;
                case WM_NCPOINTERDOWN:
                    std::cerr << "WM_NCPOINTERDOWN: " << WM_NCPOINTERDOWN << std::endl;
                    break;
                case WM_NCPOINTERUP:
                    std::cerr << "WM_NCPOINTERUP: " << WM_NCPOINTERUP << std::endl;
                    break;
                case WM_POINTERUPDATE:
                    std::cerr << "WM_POINTERUPDATE: " << WM_POINTERUPDATE << std::endl;
                    break;
                case WM_POINTERDOWN:
                    std::cerr << "WM_POINTERDOWN: " << WM_POINTERDOWN << std::endl;
                    break;
                case WM_POINTERUP:
                    std::cerr << "WM_POINTERUP: " << WM_POINTERUP << std::endl;
                    break;
                case WM_POINTERENTER:
                    std::cerr << "WM_POINTERENTER: " << WM_POINTERENTER << std::endl;
                    break;
                case WM_POINTERLEAVE:
                    std::cerr << "WM_POINTERLEAVE: " << WM_POINTERLEAVE << std::endl;
                    break;
                case WM_POINTERACTIVATE:
                    std::cerr << "WM_POINTERACTIVATE: " << WM_POINTERACTIVATE << std::endl;
                    break;
                case WM_POINTERCAPTURECHANGED:
                    std::cerr << "WM_POINTERCAPTURECHANGED: " << WM_POINTERCAPTURECHANGED
                              << std::endl;
                    break;
                case WM_TOUCHHITTESTING:
                    std::cerr << "WM_TOUCHHITTESTING: " << WM_TOUCHHITTESTING << std::endl;
                    break;
                case WM_POINTERWHEEL:
                    std::cerr << "WM_POINTERWHEEL: " << WM_POINTERWHEEL << std::endl;
                    break;
                case WM_POINTERHWHEEL:
                    std::cerr << "WM_POINTERHWHEEL: " << WM_POINTERHWHEEL << std::endl;
                    break;
                case WM_POINTERROUTEDTO:
                    std::cerr << "WM_POINTERROUTEDTO: " << WM_POINTERROUTEDTO << std::endl;
                    break;
                case WM_POINTERROUTEDAWAY:
                    std::cerr << "WM_POINTERROUTEDAWAY: " << WM_POINTERROUTEDAWAY << std::endl;
                    break;
                case WM_POINTERROUTEDRELEASED:
                    std::cerr << "WM_POINTERROUTEDRELEASED: " << WM_POINTERROUTEDRELEASED
                              << std::endl;
                    break;
                case WM_IME_SETCONTEXT:
                    std::cerr << "WM_IME_SETCONTEXT: " << WM_IME_SETCONTEXT << std::endl;
                    break;
                case WM_IME_NOTIFY:
                    std::cerr << "WM_IME_NOTIFY: " << WM_IME_NOTIFY << std::endl;
                    break;
                case WM_IME_CONTROL:
                    std::cerr << "WM_IME_CONTROL: " << WM_IME_CONTROL << std::endl;
                    break;
                case WM_IME_COMPOSITIONFULL:
                    std::cerr << "WM_IME_COMPOSITIONFULL: " << WM_IME_COMPOSITIONFULL << std::endl;
                    break;
                case WM_IME_SELECT:
                    std::cerr << "WM_IME_SELECT: " << WM_IME_SELECT << std::endl;
                    break;
                case WM_IME_CHAR:
                    std::cerr << "WM_IME_CHAR: " << WM_IME_CHAR << std::endl;
                    break;
                case WM_IME_REQUEST:
                    std::cerr << "WM_IME_REQUEST: " << WM_IME_REQUEST << std::endl;
                    break;
                case WM_IME_KEYDOWN:
                    std::cerr << "WM_IME_KEYDOWN: " << WM_IME_KEYDOWN << std::endl;
                    break;
                case WM_IME_KEYUP:
                    std::cerr << "WM_IME_KEYUP: " << WM_IME_KEYUP << std::endl;
                    break;
                case WM_MOUSEHOVER:
                    std::cerr << "WM_MOUSEHOVER: " << WM_MOUSEHOVER << std::endl;
                    break;
                case WM_MOUSELEAVE:
                    std::cerr << "WM_MOUSELEAVE: " << WM_MOUSELEAVE << std::endl;
                    break;
                case WM_NCMOUSEHOVER:
                    std::cerr << "WM_NCMOUSEHOVER: " << WM_NCMOUSEHOVER << std::endl;
                    break;
                case WM_NCMOUSELEAVE:
                    std::cerr << "WM_NCMOUSELEAVE: " << WM_NCMOUSELEAVE << std::endl;
                    break;
                case WM_WTSSESSION_CHANGE:
                    std::cerr << "WM_WTSSESSION_CHANGE: " << WM_WTSSESSION_CHANGE << std::endl;
                    break;
                case WM_TABLET_FIRST:
                    std::cerr << "WM_TABLET_FIRST: " << WM_TABLET_FIRST << std::endl;
                    break;
                case WM_TABLET_LAST:
                    std::cerr << "WM_TABLET: " << WM_TABLET_LAST << std::endl;
                    break;
                case WM_DPICHANGED:
                    std::cerr << "WM_DPICHANGED: " << WM_DPICHANGED << std::endl;
                    break;
                case WM_DPICHANGED_BEFOREPARENT:
                    std::cerr << "WM_DPICHANGED_BEFOREPARENT: " << WM_DPICHANGED_BEFOREPARENT
                              << std::endl;
                    break;
                case WM_DPICHANGED_AFTERPARENT:
                    std::cerr << "WM_DPICHANGED_AFTERPARENT: " << WM_DPICHANGED_AFTERPARENT
                              << std::endl;
                    break;
                case WM_GETDPISCALEDSIZE:
                    std::cerr << "WM_GETDPISCALEDSIZE: " << WM_GETDPISCALEDSIZE << std::endl;
                    break;
                case WM_CUT:
                    std::cerr << "WM_CUT: " << WM_CUT << std::endl;
                    break;
                case WM_COPY:
                    std::cerr << "WM_COPY: " << WM_COPY << std::endl;
                    break;
                case WM_PASTE:
                    std::cerr << "WM_PASTE: " << WM_PASTE << std::endl;
                    break;
                case WM_CLEAR:
                    std::cerr << "WM_CLEAR: " << WM_CLEAR << std::endl;
                    break;
                case WM_UNDO:
                    std::cerr << "WM_UNDO: " << WM_UNDO << std::endl;
                    break;
                case WM_RENDERFORMAT:
                    std::cerr << "WM_RENDERFORMAT: " << WM_RENDERFORMAT << std::endl;
                    break;
                case WM_RENDERALLFORMATS:
                    std::cerr << "WM_RENDERALLFORMATS: " << WM_RENDERALLFORMATS << std::endl;
                    break;
                case WM_DESTROYCLIPBOARD:
                    std::cerr << "WM_DESTROYCLIPBOARD: " << WM_DESTROYCLIPBOARD << std::endl;
                    break;
                case WM_DRAWCLIPBOARD:
                    std::cerr << "WM_DRAWCLIPBOARD: " << WM_DRAWCLIPBOARD << std::endl;
                    break;
                case WM_PAINTCLIPBOARD:
                    std::cerr << "WM_PAINTCLIPBOARD: " << WM_PAINTCLIPBOARD << std::endl;
                    break;
                case WM_VSCROLLCLIPBOARD:
                    std::cerr << "WM_VSCROLLCLIPBOARD: " << WM_VSCROLLCLIPBOARD << std::endl;
                    break;
                case WM_SIZECLIPBOARD:
                    std::cerr << "WM_SIZECLIPBOARD: " << WM_SIZECLIPBOARD << std::endl;
                    break;
                case WM_ASKCBFORMATNAME:
                    std::cerr << "WM_ASKCBFORMATNAME: " << WM_ASKCBFORMATNAME << std::endl;
                    break;
                case WM_CHANGECBCHAIN:
                    std::cerr << "WM_CHANGECBCHAIN: " << WM_CHANGECBCHAIN << std::endl;
                    break;
                case WM_HSCROLLCLIPBOARD:
                    std::cerr << "WM_HSCROLLCLIPBOARD: " << WM_HSCROLLCLIPBOARD << std::endl;
                    break;
                case WM_QUERYNEWPALETTE:
                    std::cerr << "WM_QUERYNEWPALETTE: " << WM_QUERYNEWPALETTE << std::endl;
                    break;
                case WM_PALETTEISCHANGING:
                    std::cerr << "WM_PALETTEISCHANGING: " << WM_PALETTEISCHANGING << std::endl;
                    break;
                case WM_PALETTECHANGED:
                    std::cerr << "WM_PALETTECHANGED: " << WM_PALETTECHANGED << std::endl;
                    break;
                case WM_HOTKEY:
                    std::cerr << "WM_HOTKEY: " << WM_HOTKEY << std::endl;
                    break;
                case WM_PRINT:
                    std::cerr << "WM_PRINT: " << WM_PRINT << std::endl;
                    break;
                case WM_PRINTCLIENT:
                    std::cerr << "WM_PRINTCLIENT: " << WM_PRINTCLIENT << std::endl;
                    break;
                case WM_APPCOMMAND:
                    std::cerr << "WM_APPCOMMAND: " << WM_APPCOMMAND << std::endl;
                    break;
                case WM_THEMECHANGED:
                    std::cerr << "WM_THEMECHANGED: " << WM_THEMECHANGED << std::endl;
                    break;
                case WM_CLIPBOARDUPDATE:
                    std::cerr << "WM_CLIPBOARDUPDATE: " << WM_CLIPBOARDUPDATE << std::endl;
                    break;
                case WM_DWMCOMPOSITIONCHANGED:
                    std::cerr << "WM_DWMCOMPOSITIONCHANGED: " << WM_DWMCOMPOSITIONCHANGED
                              << std::endl;
                    break;
                case WM_DWMNCRENDERINGCHANGED:
                    std::cerr << "WM_DWMNCRENDERINGCHANGED: " << WM_DWMNCRENDERINGCHANGED
                              << std::endl;
                    break;
                case WM_DWMCOLORIZATIONCOLORCHANGED:
                    std::cerr << "WM_DWMCOLORIZATIONCOLORCHANGED: "
                              << WM_DWMCOLORIZATIONCOLORCHANGED << std::endl;
                    break;
                case WM_DWMWINDOWMAXIMIZEDCHANGE:
                    std::cerr << "WM_DWMWINDOWMAXIMIZEDCHANGE: " << WM_DWMWINDOWMAXIMIZEDCHANGE
                              << std::endl;
                    break;
                case WM_DWMSENDICONICTHUMBNAIL:
                    std::cerr << "WM_DWMSENDICONICTHUMBNAIL: " << WM_DWMSENDICONICTHUMBNAIL
                              << std::endl;
                    break;
                case WM_DWMSENDICONICLIVEPREVIEWBITMAP:
                    std::cerr << "WM_DWMSENDICONICLIVEPREVIEWBITMAP: "
                              << WM_DWMSENDICONICLIVEPREVIEWBITMAP << std::endl;
                    break;
                case WM_GETTITLEBARINFOEX:
                    std::cerr << "WM_GETTITLEBARINFOEX: " << WM_GETTITLEBARINFOEX << std::endl;
                    break;
                case WM_HANDHELDFIRST:
                    std::cerr << "WM_HANDHELDFIRST: " << WM_HANDHELDFIRST << std::endl;
                    break;
                case WM_HANDHELDLAST:
                    std::cerr << "WM_HANDHELDLAST: " << WM_HANDHELDLAST << std::endl;
                    break;
                case WM_AFXFIRST:
                    std::cerr << "WM_AFXFIRST: " << WM_AFXFIRST << std::endl;
                    break;
                case WM_AFXLAST:
                    std::cerr << "WM_AFXLAST: " << WM_AFXLAST << std::endl;
                    break;
                case WM_PENWINFIRST:
                    std::cerr << "WM_PENWINFIRST: " << WM_PENWINFIRST << std::endl;
                    break;
                case WM_PENWINLAST:
                    std::cerr << "WM_PENWINLAST: " << WM_PENWINLAST << std::endl;
                    break;
                case WM_APP:
                    std::cerr << "WM_APP: " << WM_APP << std::endl;
                    break;
                case WM_USER:
                    std::cerr << "WM_USER: " << WM_USER << std::endl;
                    break;
                case WM_TOOLTIPDISMISS:
                    std::cerr << "WM_TOOLTIPDISMISS: " << WM_TOOLTIPDISMISS << std::endl;
                    break;
                default:
                    std::cerr << "Unknown message: " << message << std::endl;
                    break;
            }

            bool result = true;
            if(message == WM_CREATE)
            {
                WindowCreateData* data = static_cast<WindowCreateData*>(
                    reinterpret_cast<CREATESTRUCTW*>(l_param)->lpCreateParams);

                SetLastError(0);
                auto res =
                    SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data->obj));
                if(res == 0) //possible error
                {
                    if(::GetLastError() != 0) //error
                        result = false;
                }
            }
            else
            {
                Window* window =
                    reinterpret_cast<Window*>(GetWindowLongPtrW(handle, GWLP_USERDATA));

                try
                {
                    switch(message)
                    {
                        case WM_QUIT:
                            window->Emit(WindowSubsystemQuitEvent{});
                            break;
                        case WM_CLOSE:
                            window->Emit(WindowCloseEvent{});
                            break;
                    }
                }
                catch(...)
                {
                    Core::System::SetLastError(Window::LAST_ERROR_CODE, std::current_exception());
                    result = false;
                }
            }

            if(result == false)
                return -1;

            return DefWindowProcW(handle, message, w_param, l_param);
        }

        Window::Window(WindowSubsystem* _parent, const WindowInfo& info)
            : parent(_parent),
              current_state(info.state)
        {
            WindowCreateData data = {.obj = this};

            DWORD ex_style = 0;
            DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

            RECT rect = {.left = 0,
                         .top = 0,
                         .right = static_cast<LONG>(info.resolution.width),
                         .bottom = static_cast<LONG>(info.resolution.height)};
            if(AdjustWindowRectEx(&rect, style & ~WS_OVERLAPPED, style & WS_SYSMENU, ex_style) == 0)
                std::rethrow_exception(Core::System::GetLastError());

            auto title = Core::System::UTF8ToWide(info.title);
            HWND _handle = CreateWindowExW(ex_style,
                                           WIN32_WINDOW_CLASS_NAME,
                                           title.data(),
                                           style,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           rect.right - rect.left,
                                           rect.bottom - rect.top,
                                           nullptr,
                                           nullptr,
                                           parent->GetInstance(),
                                           &data);

            if(_handle == nullptr)
                std::rethrow_exception(Core::System::GetLastError());

            this->handle = _handle;

            Core::ScopedCall cleanup(
                [&_handle]()
                {
                    DestroyWindow(_handle);
                });

            display.reset(new Display(this, MonitorFromWindow(_handle, MONITOR_DEFAULTTONEAREST)));

            this->SetState(info.state);
            UpdatePrevWindowedState();

            cleanup.Drop();
        }

        Window::~Window()
        {
            DestroyWindow(handle);
        }

        void Window::SetTitle(std::string_view title)
        {
            auto wstr = Core::System::UTF8ToWide(title);

            if(SetWindowTextW(handle, wstr.data()) == 0)
                std::rethrow_exception(Core::System::GetLastError());
        }

        std::string Window::GetTitle() const
        {
            ::SetLastError(0); //clear last error
            int length = GetWindowTextLengthW(handle);
            if(length == 0)
            {
                auto last_error = ::GetLastError();
                if(last_error == 0) //empty title
                    return "";

                std::rethrow_exception(Core::System::GetLastError());
            }

            std::wstring wstr(length, L'\0');
            if(GetWindowTextW(handle, wstr.data(), length + 1) == 0)
                std::rethrow_exception(Core::System::GetLastError());

            return Core::System::WideToUTF8(wstr);
        }

        void Window::Resize(const WindowResolution& resolution)
        {
            if(current_state == WindowState::FullScreen)
                return;

            RECT rect = {.left = 0,
                         .top = 0,
                         .right = static_cast<LONG>(resolution.width),
                         .bottom = static_cast<LONG>(resolution.height)};

            ::SetLastError(0);
            auto style = GetWindowLongPtrW(handle, GWL_STYLE);
            if(style == 0)
            {
                if(::GetLastError() != 0)
                    std::rethrow_exception(Core::System::GetLastError());
            }

            auto ex_style = GetWindowLongPtrW(handle, GWL_EXSTYLE);
            if(ex_style == 0)
            {
                if(::GetLastError() != 0)
                    std::rethrow_exception(Core::System::GetLastError());
            }

            if(AdjustWindowRectEx(&rect, style & ~WS_OVERLAPPED, style & WS_SYSMENU, ex_style) == 0)
                std::rethrow_exception(Core::System::GetLastError());

            //ignore X and Y
            if(SetWindowPos(handle,
                            nullptr,
                            0,
                            0,
                            rect.right - rect.left,
                            rect.bottom - rect.top,
                            SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOREDRAW | SWP_NOZORDER) == 0)
            {
                std::rethrow_exception(Core::System::GetLastError());
            }

            UpdatePrevWindowedState();
        }

        WindowResolution Window::GetResolution() const
        {
            RECT rect;
            if(GetClientRect(handle, &rect) == 0)
                std::rethrow_exception(Core::System::GetLastError());

            return WindowResolution{.width = static_cast<std::uint32_t>(rect.right),
                                    .height = static_cast<std::uint32_t>(rect.bottom)};
        }

        WindowResolution Window::GetScaledResolution() const
        {
            return GetResolution();
        }

        void Window::SetState(WindowState state)
        {
            if(current_state == state)
                return;

            if(state == WindowState::FullScreen)
            {
                //get monitor rect(rcMonitor)
                auto video_mode = display->GetCurrentVideoMode();
                auto display_positon = display->GetPosition();

                //hide title bar and other gui
                ::SetLastError(0);
                auto style = GetWindowLongPtrW(handle, GWL_STYLE);
                if(style == 0)
                {
                    if(::GetLastError() != 0)
                        std::rethrow_exception(Core::System::GetLastError());
                }

                if(SetWindowLongPtrW(handle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW) == 0)
                {
                    if(::GetLastError() != 0)
                        std::rethrow_exception(Core::System::GetLastError());
                }

                //apply rect + make topmost
                if(SetWindowPos(handle,
                                HWND_BOTTOM,
                                display_positon.x,
                                display_positon.y,
                                video_mode.width,
                                video_mode.height,
                                SWP_NOREDRAW) == 0)
                    std::rethrow_exception(Core::System::GetLastError());
            }
            else
            {
                //restore style
                ::SetLastError(0);
                auto style = GetWindowLongPtrW(handle, GWL_STYLE);
                if(style == 0)
                {
                    if(::GetLastError() != 0)
                        std::rethrow_exception(Core::System::GetLastError());
                }

                if(SetWindowLongPtrW(handle, GWL_STYLE, style | WS_OVERLAPPEDWINDOW) == 0)
                {
                    if(::GetLastError() != 0)
                        std::rethrow_exception(Core::System::GetLastError());
                }

                //restore to prev windowed position and resolution
                if(SetWindowPos(handle,
                                HWND_TOP,
                                windowed_prev_position.x,
                                windowed_prev_position.y,
                                windowed_prev_resolution.width,
                                windowed_prev_resolution.height,
                                SWP_NOREDRAW) == 0)
                    std::rethrow_exception(Core::System::GetLastError());
            }

            current_state = state;
        }

        WindowState Window::GetState() const
        {
            return current_state;
        }

        void Window::SetMouseCursorPosition(const WindowPosition& pos)
        {
            POINT point = {.x = pos.x, .y = pos.y};
            if(ClientToScreen(handle, &point) == 0)
                std::rethrow_exception(Core::System::GetLastError());

            if(SetCursorPos(point.x, point.y) == 0)
                std::rethrow_exception(Core::System::GetLastError());
        }

        WindowPosition Window::GetMouseCursorPosition() const
        {
            POINT point = {};
            if(GetCursorPos(&point) == 0)
                std::rethrow_exception(Core::System::GetLastError());

            if(ScreenToClient(handle, &point) == 0)
                std::rethrow_exception(Core::System::GetLastError());

            return WindowPosition{.x = point.x, .y = point.y};
        }

        WindowSurfaceInfo Window::GetWindowSurfaceInfo() const noexcept
        {
            return Render::Win32SurfaceInfo{.instance = parent->GetInstance(), .window = handle};
        }

        Display* Window::GetDisplay() const noexcept
        {
            return display.get();
        }

        Core::WindowSubsystem* Window::GetParent() const noexcept
        {
            return parent;
        }

        void Window::UpdatePrevWindowedState()
        {
            RECT rect;
            if(GetWindowRect(handle, &rect) == 0)
                std::rethrow_exception(Core::System::GetLastError());

            windowed_prev_position = {.x = rect.left, .y = rect.top};
            windowed_prev_resolution = {.width = static_cast<std::uint32_t>(rect.right - rect.left),
                                        .height =
                                            static_cast<std::uint32_t>(rect.bottom - rect.top)};
        }
    };
};
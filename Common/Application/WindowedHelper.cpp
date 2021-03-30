#include "CommonPCH.h"

#include "Common/Application/WindowedHelper.h"
#include "Common/Application/iApplication.h"
#include "Common/Application/log.h"
#include "Common/Utils/Utf8.h"

#define DSC_DXTK12 1
#if defined(DSC_DXTK12)
   #include "Common/DirectXTK12/Keyboard.h"
   #include "Common/DirectXTK12/Mouse.h"
#endif //#if defined(DSC_DXTK12)

static int s_windowDefaultWidth = 0;
static int s_windowDefaultHeight = 0;
static bool s_in_sizemove = false;
static bool s_in_suspend = false;
static bool s_minimized = false;
static bool s_fullscreen = false;

// Windows procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    auto pApplication = reinterpret_cast<iApplication*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_PAINT:
        if (s_in_sizemove && pApplication)
        {
            pApplication->Tick();
        }
        else
        {
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_MOVE:
        if (pApplication)
        {
            pApplication->OnWindowMoved();
        }
        break;

    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            if (!s_minimized)
            {
                s_minimized = true;
                if (!s_in_suspend && pApplication)
                {
                    pApplication->OnSuspending();
                }
                s_in_suspend = true;
            }
        }
        else if (s_minimized)
        {
            s_minimized = false;
            if (s_in_suspend && pApplication)
            {
                pApplication->OnResuming();
            }
            s_in_suspend = false;
        }
        else if (!s_in_sizemove && pApplication)
        {
            pApplication->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
        }
        break;

    case WM_ENTERSIZEMOVE:
        s_in_sizemove = true;
        break;

    case WM_EXITSIZEMOVE:
        s_in_sizemove = false;
        if (pApplication)
        {
            RECT rc;
            GetClientRect(hWnd, &rc);

            pApplication->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
        }
        break;

    case WM_GETMINMAXINFO:
        {
            auto info = reinterpret_cast<MINMAXINFO*>(lParam);
            info->ptMinTrackSize.x = 320;
            info->ptMinTrackSize.y = 200;
        }
        break;

    case WM_ACTIVATEAPP:
        if (pApplication)
        {
#if defined(DSC_DXTK12)
            DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
            DirectX::Mouse::ProcessMessage(message, wParam, lParam);
#endif //#if defined(DSC_DXTK12)

            if (wParam)
            {
                pApplication->OnActivated();
            }
            else
            {
                pApplication->OnDeactivated();
            }
        }
        break;

    case WM_POWERBROADCAST:
        switch (wParam)
        {
        case PBT_APMQUERYSUSPEND:
            if (!s_in_suspend && pApplication)
            {
                pApplication->OnSuspending();
            }
            s_in_suspend = true;
            return TRUE;

        case PBT_APMRESUMESUSPEND:
            if (!s_minimized)
            {
                if (s_in_suspend && pApplication)
                {
                    pApplication->OnResuming();
                }
                s_in_suspend = false;
            }
            return TRUE;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

#if defined(DSC_DXTK12)
    case WM_INPUT:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_MOUSEHOVER:
        DirectX::Mouse::ProcessMessage(message, wParam, lParam);
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
        break;
#endif //#if defined(DSC_DXTK12)

    case WM_SYSKEYDOWN:
        //LOG_MESSAGE_DEBUG("WM_SYSKEYDOWN\n");
        if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
        {
             LOG_MESSAGE_DEBUG("WM_SYSKEYDOWN ALT+ENTER\n");
           // Implements the classic ALT+ENTER fullscreen toggle
            if (s_fullscreen)
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

                //LOG_MESSAGE_DEBUG(L"fullscreen::ShowWindow\n");
                ShowWindow(hWnd, SW_SHOWNORMAL);
                LOG_MESSAGE_DEBUG("fullscreen::SetWindowPos\n");

                RECT rc = { 0, 0, static_cast<LONG>(s_windowDefaultWidth), static_cast<LONG>(s_windowDefaultHeight) };
                AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
                SetWindowPos(hWnd, HWND_TOP, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
            else
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, 0);
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

                LOG_MESSAGE_DEBUG("notfullscreen::SetWindowPos\n");
                SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                LOG_MESSAGE_DEBUG("notfullscreen::ShowWindow\n");
                ShowWindow(hWnd, SW_SHOWMAXIMIZED);
            }

            s_fullscreen = !s_fullscreen;
        }
#if defined(DSC_DXTK12)
        DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
#endif //#if defined(DSC_DXTK12)
        break;

    case WM_MENUCHAR:
        // A menu is active and the user presses a key that does not correspond
        // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
        return MAKELRESULT(0, MNC_CLOSE);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

static void ConsumeCmdLine(std::vector< std::wstring >& cmdLineArray, LPWSTR lpCmdLine)
{
   LOG_MESSAGE_DEBUG("ConsumeCmdLine\n");

   int nArgs = 0;
   LPWSTR *szArglist = CommandLineToArgvW(lpCmdLine, &nArgs);
   if( NULL == szArglist )
   {
      return;
   }
   for( int index=0; index<nArgs; index++)
   {
      cmdLineArray.push_back(std::wstring(szArglist[index]));
      LOG_MESSAGE_DEBUG("  %s\n", Utf8::Utf16ToUtf8(szArglist[index]).c_str());
   }
   LocalFree(szArglist);
}

const int WindowedHelper(
   _In_ HINSTANCE hInstance, 
   _In_ LPWSTR lpCmdLine,
   _In_ int nCmdShow,
   const std::wstring& className,
   const std::wstring& windowTitle,
   const int defaultWindowWidth,
   const int defaultWindowHeight,
   const bool bFullScreen,
   const std::function< std::unique_ptr< iApplication >(const std::vector< std::wstring >& cmdLineArray, HWND hwnd, const int width, const int height) >& applicationFactory
   )
{
   LOG_SCOPE_DEBUG("WindowedHelper");

    if (!DirectX::XMVerifyCPUSupport())
        return 1;

    s_fullscreen = bFullScreen;
    s_windowDefaultWidth = defaultWindowWidth;
    s_windowDefaultHeight = defaultWindowHeight;

    std::vector< std::wstring > cmdLineArray;
    ConsumeCmdLine(cmdLineArray, lpCmdLine);

    std::unique_ptr< iApplication > pApplication;

    // Register class and create window
    {
        // Register class
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIconW(hInstance, L"IDI_ICON");
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
        wcex.lpszClassName = className.c_str();
        wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
        if (!RegisterClassExW(&wcex))
            return 1;

        // Create window
        RECT rc = { 0, 0, static_cast<LONG>(defaultWindowWidth), static_cast<LONG>(defaultWindowHeight) };

        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        const int exStyle = bFullScreen ? WS_EX_TOPMOST : 0;
        const int style = bFullScreen ? WS_POPUP : WS_OVERLAPPEDWINDOW;

        HWND hwnd = CreateWindowExW(exStyle, className.c_str(), windowTitle.c_str(), style,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
            nullptr);

        if (!hwnd)
        {
           LOG_MESSAGE_WARNING("creation of window failed");
           return 1;
        }
        {
           const int localCmdShow = bFullScreen ? SW_SHOWMAXIMIZED : nCmdShow;
           ShowWindow(hwnd, localCmdShow);
        }

        GetClientRect(hwnd, &rc);
        pApplication = applicationFactory ? applicationFactory(cmdLineArray, hwnd, rc.right - rc.left, rc.bottom - rc.top) : nullptr;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApplication.get()) );
    }

    // Main message loop
    MSG msg = {};
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
           if (nullptr != pApplication)
           {
               pApplication->Tick();
           }
        }
    }

    pApplication.reset();

    return (int) msg.wParam;
}

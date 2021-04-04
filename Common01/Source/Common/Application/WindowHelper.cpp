#include "CommonPCH.h"
#include "Common/Application/WindowHelper.h"
#include "Common/Application/TaskHolder.h"
#include "Common/Application/IApplication.h"
#include "Common/Util/Utf8.h"
#include "Common/Log/Log.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

struct CreateData
{
   CreateData(
      const std::shared_ptr<TaskHolder>& pTaskHolder,
      const std::function< IApplication*(HWND, const std::shared_ptr<TaskHolder>&) >& pApplicationFactory
      )
      : m_pTaskHolder(pTaskHolder)
      , m_pApplicationFactory(pApplicationFactory)
   {
      //nop
   }
   std::shared_ptr<TaskHolder> m_pTaskHolder;
   std::function< IApplication*(HWND, const std::shared_ptr<TaskHolder>&) > m_pApplicationFactory;
};

const int WindowHelper(
   const std::shared_ptr<TaskHolder>& pTaskHolder,
   const std::function< IApplication*(HWND, const std::shared_ptr<TaskHolder>&) >& pApplicationFactory,
   HINSTANCE hInstance,
   const std::string& applicationName,
   const bool bFullScreen,
   const int defaultWidth,
   const int defaultHeight,
   const int nCmdShow
   )
{
   {
      auto pApplication = pApplicationFactory(0, pTaskHolder);
      delete pApplication;
   }

   const std::wstring className(Utf8::Utf8ToUtf16( applicationName + std::string("Class")));
   const std::wstring name(Utf8::Utf8ToUtf16( applicationName ));
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
        wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wcex.lpszClassName = className.c_str();
        wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
        if (!RegisterClassExW(&wcex))
            return 1;

        // Create window

        RECT rc = { 0, 0, static_cast<LONG>(defaultWidth), static_cast<LONG>(defaultHeight) };

        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        HWND hwnd = 0;
        CreateData createData(pTaskHolder, pApplicationFactory);
        if (true == bFullScreen)
        {
            hwnd = CreateWindowExW(WS_EX_TOPMOST, className.c_str(), name.c_str(), WS_POPUP,
               CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
               &createData);
        }
        else
        {
            hwnd = CreateWindowExW(0, className.c_str(), name.c_str(), WS_OVERLAPPEDWINDOW,
               CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
               &createData);
        }

        if (!hwnd)
        {
            return 1;
        }

        //pTaskHolder->AddHwnd(hwnd);

        if (true == bFullScreen)
        {
           ShowWindow(hwnd, SW_SHOWMAXIMIZED);
        }
        else
        {
           ShowWindow(hwnd, nCmdShow);
        }
        // TODO: Change nCmdShow to SW_SHOWMAXIMIZED to default to fullscreen.

        //SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_game.get()));

        //GetClientRect(hwnd, &rc);

        //g_game->Initialize(hwnd, rc.right - rc.left, rc.bottom - rc.top);
    }

    // Main message loop
    //MSG msg = {};
    //while (WM_QUIT != msg.message)
    //{
    //    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    //    {
    //        TranslateMessage(&msg);
    //        DispatchMessage(&msg);
    //    }
    //    else
    //    {
    //        //g_game->Tick();
    //    }
    //}

    //XGameRuntimeUninitialize();

    //g_game.reset();

    return 0; //static_cast<int>(msg.wParam);
}

// Windows procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static bool s_in_sizemove = false;
    static bool s_in_suspend = false;
    static bool s_minimized = false;
    static bool s_fullscreen = false;
    // TODO: Set s_fullscreen to true if defaulting to fullscreen.

    auto pApplication = reinterpret_cast<IApplication*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_CREATE:
       {
         auto pData = (LPCREATESTRUCTA)(lParam);
         CreateData* pCreateData = pData ? (CreateData*)(pData->lpCreateParams) : nullptr;
         if (pCreateData)
         {
            auto pApplicationNew = pCreateData->m_pApplicationFactory(
               hWnd,
               pCreateData->m_pTaskHolder
               );
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pApplicationNew);
         }
       }
       break;
    case WM_PAINT:
        //if (s_in_sizemove && game)
        //{
        //    //game->Tick();
        //}
        //else
        {
            PAINTSTRUCT ps;
            (void)BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_MOVE:
        //if (game)
        //{
        //    game->OnWindowMoved();
        //}
        break;

    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            if (!s_minimized)
            {
                s_minimized = true;
                //if (!s_in_suspend && game)
                //    game->OnSuspending();
                s_in_suspend = true;
            }
        }
        else if (s_minimized)
        {
            s_minimized = false;
            //if (s_in_suspend && game)
            //    game->OnResuming();
            s_in_suspend = false;
        }
        //else if (!s_in_sizemove && game)
        //{
        //    game->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
        //}
        break;

    case WM_ENTERSIZEMOVE:
        s_in_sizemove = true;
        break;

    case WM_EXITSIZEMOVE:
        s_in_sizemove = false;
        //if (game)
        //{
        //    RECT rc;
        //    GetClientRect(hWnd, &rc);
        //
        //    game->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
        //}
        break;

    case WM_GETMINMAXINFO:
        if (lParam)
        {
            auto info = reinterpret_cast<MINMAXINFO*>(lParam);
            info->ptMinTrackSize.x = 320;
            info->ptMinTrackSize.y = 200;
        }
        break;

    case WM_ACTIVATEAPP:
        //if (game)
        //{
        //    if (wParam)
        //    {
        //        game->OnActivated();
        //    }
        //    else
        //    {
        //        game->OnDeactivated();
        //    }
        //}
        break;

    case WM_POWERBROADCAST:
        switch (wParam)
        {
        case PBT_APMQUERYSUSPEND:
            //if (!s_in_suspend && game)
            //    game->OnSuspending();
            s_in_suspend = true;
            return TRUE;

        case PBT_APMRESUMESUSPEND:
            if (!s_minimized)
            {
                //if (s_in_suspend && game)
                //    game->OnResuming();
                s_in_suspend = false;
            }
            return TRUE;
        }
        break;

    case WM_DESTROY:
       {
         if (pApplication)
         {
            delete pApplication;
            pApplication = nullptr;
         }
         SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)0);
         PostQuitMessage(0);
       }
        break;

    case WM_SYSKEYDOWN:
        if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
        {
            // Implements the classic ALT+ENTER fullscreen toggle
            if (s_fullscreen)
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

                int width = 800;
                int height = 600;
                //if (game)
                //    game->GetDefaultSize(width, height);

                ShowWindow(hWnd, SW_SHOWNORMAL);

                SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
            else
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, 0);
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

                SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

                ShowWindow(hWnd, SW_SHOWMAXIMIZED);
            }

            s_fullscreen = !s_fullscreen;
        }
        break;

    case WM_MENUCHAR:
        // A menu is active and the user presses a key that does not correspond
        // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
        return MAKELRESULT(0, MNC_CLOSE);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

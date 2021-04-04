//
// Main.cpp
//

#include "ApplicationPCH.h"
#include "Common/Application/WindowHelper.h"
#include "Common/Application/CommandLine.h"
#include "Common/Util/Utf8.h"
#include "Common/Log/Log.h"
#include "Common/Log/LogConsumerConsole.h"

LPCWSTR g_szAppName = L"Application01";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (!DirectX::XMVerifyCPUSupport())
    {
        return -1;
    }

    // Initialize the GameRuntime
    HRESULT hr = XGameRuntimeInitialize();
    if (FAILED(hr))
    {
        if (hr == E_GAMERUNTIME_DLL_NOT_FOUND || hr == E_GAMERUNTIME_VERSION_MISMATCH)
        {
            (void)MessageBoxW(nullptr, L"Game Runtime is not installed on this system or needs updating.", L"Main", MB_ICONERROR | MB_OK);
        }
        return -1;
    }
    LogConsumerConsole logConsumerConsole;
    auto pCommandLine = CommandLine::Factory(Utf8::Utf16ToUtf8(GetCommandLineW()));

    const int result = WindowHelper(
      hInstance,
      "Application01",
       false,
       800,
       600,
       nCmdShow
      );

    XGameRuntimeUninitialize();

    return result;
}

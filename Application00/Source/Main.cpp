//
// Main.cpp
//

#include "PCH.h"
#include "Common/Application/WindowedHelper.h"
#include "Application.h"

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    const int exitCode = WindowedHelper(
       hInstance,
       nCmdShow,
       L"Appliaction00Class", 
       L"Appliaction00", 
       800, 
       600,
       false,
       [](HWND hwnd, const int width, const int height){ return std::make_unique< Application >(hwnd, width, height); }
      );

    return exitCode;
}

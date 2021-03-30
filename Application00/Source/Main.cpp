//
// Main.cpp
//

#include "PCH.h"
#include "Common/Application/WindowedHelper.h"
#include "Common/Application/Log.h"
#include "Application.h"

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(lpCmdLine);

#if defined(_DEBUG)
   LogConsumerWriteToFile logWriteToFile;
#endif
   LogConsumerConsole logConsole;

   const int exitCode = WindowedHelper(
      hInstance,
      GetCommandLineW(),
      nCmdShow,
      L"Application00Class", 
      L"Application00", 
      1440, 
      810,
      false,
      [](const std::vector< std::wstring >& cmdLineArray, HWND hwnd, const int width, const int height){ return std::make_unique< Application >(cmdLineArray, hwnd, width, height); }
      );

   return exitCode;
}

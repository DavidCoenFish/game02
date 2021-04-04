//
// Main.cpp
//

#include "ApplicationPCH.h"
#include "Common/Application/WindowHelper.h"
#include "Common/Application/CommandLine.h"
#include "Common/Util/Utf8.h"
#include "Common/Log/Log.h"
#include "Common/Log/LogConsumerConsole.h"
#include "json/json.hpp"

//#define MAX_PATH_LENGTH MAX_PATH 
#define MAX_PATH_LENGTH 1024 
static const std::filesystem::path GetModuleFilePath(HINSTANCE hInstance)
{
   wchar_t data[MAX_PATH_LENGTH];
   const auto length = GetModuleFileNameW(hInstance, &data[0], MAX_PATH_LENGTH);
   if (length == MAX_PATH_LENGTH)
   {
      return std::string();
   }
   std::string exePath = Utf8::Utf16ToUtf8(data);
   std::filesystem::path path(exePath);
   path.remove_filename();
   return path;
}

static const int RunTask(HINSTANCE hInstance, int nCmdShow)
{
#if defined(DSC_LOG)
    std::vector< std::shared_ptr< ILogConsumer > > arrayLogs;
    arrayLogs.push_back(std::make_shared<LogConsumerConsole>());
#endif

    LogConsumerConsole logConsumerConsole;
    auto pCommandLine = CommandLine::Factory(Utf8::Utf16ToUtf8(GetCommandLineW()));
    if (nullptr == pCommandLine)
    {
       return -1;
    }

    const auto basePath = GetModuleFilePath(hInstance);
    if (2 <= pCommandLine->GetParamCount())
    {
       std::filesystem::path filePath = basePath / "Task" / pCommandLine->GetParam(1);

      auto applicationJson = nlohmann::json::parse( 2 < cmdLineArray.size() ? FileCache::RawLoadFileString(cmdLineArray[1]) : "{}");
    }

    //const int result = WindowHelper(
    //  hInstance,
    //  "Application01",
    //   false,
    //   800,
    //   600,
    //   nCmdShow
    //  );

    return result;
}

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

    const int result = RunTask(hInstance, nCmdShow);

    XGameRuntimeUninitialize();

    return result;
}

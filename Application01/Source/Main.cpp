//
// Main.cpp
//

#include "ApplicationPCH.h"
#include "Common/Application/WindowHelper.h"
#include "Common/Application/CommandLine.h"
#include "Common/FileSystem/FileSystem.h"
#include "Common/FileSystem/ReadOverlayDir.h"
#include "Common/Log/Log.h"
#include "Common/Log/LogConsumerConsole.h"
#include "Common/Util/Utf8.h"
#include "json/json.hpp"

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

    const auto basePath = FileSystem::GetModualDir(hInstance);
    FileSystem fileSystem;
    fileSystem.AddReadOverlay( std::make_shared< ReadOverlayDir >( 0, basePath ) );

    if (2 <= pCommandLine->GetParamCount())
    {
       std::filesystem::path filePath = basePath / "Task" / pCommandLine->GetParam(1);

      //auto applicationJson = nlohmann::json::parse( 2 < cmdLineArray.size() ? FileCache::RawLoadFileString(cmdLineArray[1]) : "{}");
    }

    const int result = WindowHelper(
      hInstance,
      "Application01",
       false,
       800,
       600,
       nCmdShow
      );

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

//
// Main.cpp
//

#include "ApplicationPCH.h"
#include "Common/Application/WindowHelper.h"
#include "Common/Application/CommandLine.h"
#include "Common/Application/IApplication.h"
#include "Common/Application/ApplicationBasic.h"
#include "Common/Application/ApplicationTestTriangle.h"
#include "Common/Application/ApplicationDisplayList.h"
#include "Common/Application/ApplicationHolder.h"
#include "Common/FileSystem/FileSystem.h"
#include "Common/FileSystem/ReadOverlayDir.h"
#include "Common/FileSystem/WriteOverlayDir.h"
#include "Common/Log/Log.h"
#include "Common/Log/LogConsumerConsole.h"
#include "Common/Util/Utf8.h"
#include "json/json.hpp"

class JSONWindow
{
public:
   JSONWindow()
      : fullScreen(false)
      , width(800)
      , height(600)
   {
      //nop
   }
   std::string name;
   bool fullScreen;
   int width;
   int height;
   std::string factory;
   std::string data;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONWindow,
   name,
   fullScreen,
   width,
   height,
   factory,
   data
   );

struct JSONApplication
{
   std::vector< JSONWindow > windows;
   //std::vector< std::string > logs;
   //std::vector< JSONLog > Logs;
   //std::vector< JSONOverlayRead > OverlayRead; // beter to be under application or data control?
   //std::vector< JSONOverlayWrite > OverlayWrite; // beter to be under application or data control?
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONApplication, 
   windows
   //Logs
   );

static const std::function< IApplication*(const IApplicationParam&) > GetFactory(const std::string& factoryKey, const std::filesystem::path& rootPath, const std::string& data)
{
   if (factoryKey == "ApplicationBasic")
   {
      return [](const IApplicationParam& applicationParam)
      {
         return new ApplicationBasic(applicationParam);
      };
   }
   else if (factoryKey == "ApplicationTestTriangle")
   {
      return [=](const IApplicationParam& applicationParam)
      {
         return new ApplicationTestTriangle(applicationParam, rootPath);
      };
   }
   else if (factoryKey == "ApplicationDisplayList")
   {
      return [=](const IApplicationParam& applicationParam)
      {
         return new ApplicationDisplayList(applicationParam, rootPath, data);
      };
   }
   return [](const IApplicationParam& applicationParam)
   {
      return new IApplication(applicationParam);
   };
}


static const int RunTask(HINSTANCE hInstance, int nCmdShow)
{
#if defined(DSC_LOG)
   std::vector< std::shared_ptr< ILogConsumer > > arrayLogs;
   arrayLogs.push_back(std::make_shared<LogConsumerConsole>());
#endif

   auto pCommandLine = CommandLine::Factory(Utf8::Utf16ToUtf8(GetCommandLineW()));
   if (nullptr == pCommandLine)
   {
      return -1;
   }

   const auto basePath = FileSystem::GetModualDir(hInstance);

   FileSystem::AddReadOverlay( std::make_shared< ReadOverlayDir >( 0, basePath ) );
   FileSystem::AddReadOverlay( std::make_shared< ReadOverlayDir >( 1, FileSystem::GetTempDir() ) );
   FileSystem::AddWriteOverlay( std::make_shared< WriteOverlayDir >( 1, FileSystem::GetTempDir() ) );

   int result = 0;

   if (2 <= pCommandLine->GetParamCount())
   {
      std::filesystem::path path = std::filesystem::path("Task") / pCommandLine->GetParam(1);
      std::filesystem::path applicationPath = path / "Application.json";
      auto pString = FileSystem::GetFileString(applicationPath);
      auto json = nlohmann::json::parse( pString ? *pString : "{}");
      JSONApplication applicationData;
      json.get_to(applicationData);

      auto pApplicationHolder = std::make_shared<ApplicationHolder>();
      for(const auto& item : applicationData.windows)
      {
         result = WindowHelper(
            pApplicationHolder,
            GetFactory(item.factory, path, item.data),
            hInstance,
            item.name,
            item.fullScreen,
            item.width,
            item.height,
            pCommandLine,
            nCmdShow
            );
      }

      //while we have windows, keep pushing messages
      MSG msg = {};
      while (true == pApplicationHolder->HasApplication())
      {
         if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
         {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
         }
         else
         {
            pApplicationHolder->Update();
         }
      }
   }
   else
   {
      LOG_MESSAGE_ERROR("Only got [%d] param, want at least a task name", pCommandLine->GetParamCount());
   }

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

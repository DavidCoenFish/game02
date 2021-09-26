//
// Main.cpp
//

#include "ApplicationPCH.h"
#include "Common/Application/CommandLine.h"
#include "Common/Task/ITask.h"
#include "Common/Task/TaskWindow.h"
#include "Common/FileSystem/FileSystem.h"
#include "Common/Log/Log.h"
#include "Common/Log/LogConsumerConsole.h"
#include "Common/Util/Utf8.h"
#include "json/json.hpp"

class JSONTask
{
public:
   std::string factoryKey;
   nlohmann::json data;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONTask,
   factoryKey,
   data
   );

struct JSONApplication
{
   std::vector< JSONTask > tasks;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONApplication, 
   tasks
   );

static std::map< std::string, TTaskFactory >& GetTaskFactoryMap()
{
   static std::map< std::string, TTaskFactory > s_map(
   {
      {"Window", TaskWindow::Factory}
   });
   return s_map;
}

//this might not work from static lib? at least not witout something to make sure the code is not marked as dead and ignored
void RegisterTaskFactory(const std::string& taskFactoryKey, const TTaskFactory& taskFactory)
{
   auto& factoryMap = GetTaskFactoryMap();
   factoryMap[taskFactoryKey] = taskFactory;
}

//i don't want to have to know all of this in main, add a RegisterApplicationFactory? can that be done in a static lib? lets find out
static const TTaskFactory GetTaskFactory(const std::string& taskFactoryKey)
{
   const auto& factoryMap = GetTaskFactoryMap();
   auto found = factoryMap.find(taskFactoryKey);
   if (found != factoryMap.end())
   {
      return found->second;
   }
   return nullptr;
}

static const int RunTask(HINSTANCE hInstance, int nCmdShow)
{
   auto pLog = Log::Factory(std::vector< std::shared_ptr< ILogConsumer >>({ 
      std::make_shared< LogConsumerConsole >()
      }));

   auto pCommandLine = CommandLine::Factory(Utf8::Utf16ToUtf8(GetCommandLineW()));
   if (nullptr == pCommandLine)
   {
      return -1;
   }

   int result = 0;
   std::string task("Empty");

   if (2 <= pCommandLine->GetParamCount())
   {
      task = pCommandLine->GetParam(1);
   }
   else
   {
      LOG_MESSAGE_ERROR("Only got [%d] param, want at least a task name", pCommandLine->GetParamCount());
   }

   {
      std::filesystem::path path = FileSystem::GetModualDir(hInstance) / "Task" / task;
      std::filesystem::path applicationPath = path / "Application.json";
      auto fileString = FileSystem::DataToString(FileSystem::SyncReadFile(applicationPath));
      auto json = nlohmann::json::parse( fileString );
      JSONApplication applicationData;
      json.get_to(applicationData);

      for (const auto& item: applicationData.tasks)
      {
         auto taskFactory = GetTaskFactory(item.factoryKey);
         auto pTask = (nullptr != taskFactory) ? taskFactory(hInstance, nCmdShow, pCommandLine, path, item.data) : nullptr;
         if (pTask)
         {
            result = pTask->Run();
         }
         if (result < 0)
         {
            LOG_MESSAGE_ERROR("task returner [%d], abort run", result);
         }
      }
   }

   return result;
}

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(lpCmdLine);

#if 0
   //push directx code down into tasks that use DirectX?
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
#endif
   const int result = RunTask(hInstance, nCmdShow);

#if 0
   //push directx code down into tasks that use DirectX?
   XGameRuntimeUninitialize();
#endif

   return result;
}

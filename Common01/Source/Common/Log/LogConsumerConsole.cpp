#include "CommonPCH.h"

#include "Common/Log/LogConsumerConsole.h"
#include "Common/Log/Log.h"

LogConsumerConsole::LogConsumerConsole()
{
   m_logConsumer = std::make_shared< std::function< void(const int, const std::string&) > >([=](const int topic, const std::string& message)
   {
      this->Consumer(topic, message);
   });
   Log::AddLogConsumer(m_logConsumer);

   if (TRUE == AttachConsole(ATTACH_PARENT_PROCESS))
   {
      FILE* pFileOut = nullptr;
      freopen_s(&pFileOut, "conout$","w",stdout);
      LOG_MESSAGE("AttachConsole");
   }
}

LogConsumerConsole::~LogConsumerConsole()
{
   Log::RemoveLogConsumer(m_logConsumer);
}

void LogConsumerConsole::Consumer(const int topic, const std::string& message )
{
   std::string output = std::to_string(topic) + ":" + message;
   OutputDebugStringA(output.c_str());
   printf(output.c_str());
}

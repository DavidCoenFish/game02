#include "CommonPCH.h"

#include "Common/Log/LogConsumerConsole.h"
#include "Common/Log/Log.h"
#include "Common/Util/Utf8.h"

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

void LogConsumerConsole::Consumer(const int topic, const std::string& messageTest )
{
   std::string text = std::to_string(topic) + std::string(":") + messageTest + "\n";
   OutputDebugStringW(Utf8::Utf8ToUtf16(text).c_str());
   printf(text.c_str());
}

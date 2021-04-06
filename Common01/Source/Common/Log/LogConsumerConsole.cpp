#include "CommonPCH.h"

#include "Common/Log/LogConsumerConsole.h"
#include "Common/Log/Log.h"
#include "Common/Util/Utf8.h"

LogConsumerConsole::LogConsumerConsole()
{
   Log::AddLogConsumer(*this);

   if (TRUE == AttachConsole(ATTACH_PARENT_PROCESS))
   {
      FILE* pFileOut = nullptr;
      freopen_s(&pFileOut, "conout$","w",stdout);
      LOG_MESSAGE("AttachConsole");
   }
}

LogConsumerConsole::~LogConsumerConsole()
{
   Log::RemoveLogConsumer(*this);
}

void LogConsumerConsole::AddMessage(const int topic, const std::string& message )
{
   std::string text = std::to_string(topic) + std::string(":") + message + "\n";
   OutputDebugStringW(Utf8::Utf8ToUtf16(text).c_str());
   printf(text.c_str());
}

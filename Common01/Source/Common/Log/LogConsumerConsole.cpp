#include "CommonPCH.h"

#include "Common/Log/LogConsumerConsole.h"
#include "Common/Log/Log.h"
#include "Common/Util/Utf8.h"

LogConsumerConsole::LogConsumerConsole()
{
   if (TRUE == AttachConsole(ATTACH_PARENT_PROCESS))
   {
      FILE* pFileOut = nullptr;
      freopen_s(&pFileOut, "conout$","w",stdout);
      LOG_MESSAGE("AttachConsole");
   }
}

LogConsumerConsole::~LogConsumerConsole()
{
   //nop
}

void LogConsumerConsole::AddMessage(const LogTopic topic, const std::string& message )
{
   std::string text = std::to_string((int)topic) + std::string(":") + message + "\n";
   OutputDebugStringW(Utf8::Utf8ToUtf16(text).c_str());
   printf(text.c_str());
}

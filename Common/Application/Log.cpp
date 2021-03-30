#include "CommonPCH.h"

#include "Common/Application/Log.h"

//this doesn't gain much in this case, is a patteren to lazy init static vectors
static std::vector< std::shared_ptr< std::function< void(const std::string&) > > >& GetFunctorArray()
{
   static std::vector< std::shared_ptr< std::function< void(const std::string&) > > > s_functorArray;
   return s_functorArray;
}

static void AddMessageInternal(const std::string& message)
{
   const auto& functorArray = GetFunctorArray();
   for (const auto& iter : functorArray)
   {
      (*iter)(message);
   }
   return;
}

void Log::AddMessage(const char* const format, ... )
{
   va_list argptr;
   va_start(argptr, format);
   int size = vsnprintf(nullptr, 0, format, argptr) + 1;
   if (0 < size)
   {
      std::string message;
      message.reserve(size);
      vsnprintf_s(message.data(), size, _TRUNCATE, format, argptr);
      AddMessageInternal(message);
   }
   va_end(argptr);
}

void Log::AddLogConsumer(
   const std::shared_ptr< std::function< void(const std::string&) > >& pLogConsumer
   )
{
   auto& functorArray = GetFunctorArray();
   functorArray.push_back(pLogConsumer);
}

void Log::RemoveLogConsumer(
   const std::shared_ptr< std::function< void(const std::string&) > >& pLogConsumer
   )
{
   auto& functorArray = GetFunctorArray();
   auto iter = std::remove(functorArray.begin(), functorArray.end(), pLogConsumer);
   iter;
}

LogConsumerWriteToFile::LogConsumerWriteToFile(const std::string& logFilePath)
{
   m_file.open(logFilePath.c_str(), std::ofstream::out);
   m_logConsumer = std::make_shared< std::function< void(const std::string&) > >([=](const std::string& message)
   {
      this->Consumer(message);
   });
   Log::AddLogConsumer(m_logConsumer);
}

LogConsumerWriteToFile::~LogConsumerWriteToFile()
{
   Log::RemoveLogConsumer(m_logConsumer);
   if (true == m_file.is_open())
   {
      m_file.flush();
      m_file.close();
   }
}

const std::string LogConsumerWriteToFile::GetDefaultPath()
{
   return "g:\\log.txt";
}

void LogConsumerWriteToFile::Consumer( const std::string& message )
{
   if (true == m_file.is_open())
   {
      m_file << message.c_str();
   }
}

LogConsumerConsole::LogConsumerConsole()
{
   m_logConsumer = std::make_shared< std::function< void(const std::string&) > >([=](const std::string& message)
   {
      this->Consumer(message);
   });
   Log::AddLogConsumer(m_logConsumer);

   if (TRUE == AttachConsole(ATTACH_PARENT_PROCESS))
   {
      FILE* pFileOut = nullptr;
      freopen_s(&pFileOut, "conout$","w",stdout);
      Log::AddMessage("AttachConsole");
   }
}

LogConsumerConsole::~LogConsumerConsole()
{
   Log::RemoveLogConsumer(m_logConsumer);
}

void LogConsumerConsole::Consumer( const std::string& message )
{
   OutputDebugStringA(message.c_str());
   printf(message.c_str());
}

LogScope::LogScope(const char* const pName)
   : m_pName(pName)
{
   LOG_MESSAGE("Scope start %s\n", m_pName);
}

LogScope::~LogScope()
{
   LOG_MESSAGE("Scope end %s\n", m_pName);
}



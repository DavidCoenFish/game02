#pragma once

#define MACRO_JOIN(a, b) MACRO_JOIN_INNER(a, b)
#define MACRO_JOIN_INNER(a, b) a ## b

#if defined(_DEBUG)
   #define LOG_SCOPE_DEBUG(name) LogScope MACRO_JOIN(_logScope,__LINE__)(name)
   #define DSC_LOG
#else
   #define LOG_SCOPE_DEBUG(name) (void)0
#endif

#if defined(DSC_LOG)
   #define LOG_MESSAGE(format, ...) Log::AddMessage(format, ##__VA_ARGS__)
   #define LOG_MESSAGE_DEBUG(format, ...) Log::AddMessage(format, ##__VA_ARGS__)
   #define LOG_MESSAGE_WARNING(format, ...) Log::AddMessage(format, ##__VA_ARGS__)
   #define LOG_MESSAGE_ERROR(format, ...) Log::AddMessage(format, ##__VA_ARGS__)
   #define LOG_MESSAGE_RENDER(format, ...) Log::AddMessage(format, ##__VA_ARGS__)
#else
   #define LOG_MESSAGE(format, ...) (void)0
   #define LOG_MESSAGE_DEBUG(format, ...) (void)0
   #define LOG_MESSAGE_WARNING(format, ...) (void)0
   #define LOG_MESSAGE_ERROR(format, ...) (void)0
   #define LOG_MESSAGE_RENDER(format, ...) (void)0
#endif

namespace Log
{
   void AddMessage(const char* const pFormat, ... );

   void AddLogConsumer(
      const std::shared_ptr< std::function< void(const std::string&) > >& pLogConsumer
      );
   void RemoveLogConsumer(
      const std::shared_ptr< std::function< void(const std::string&) > >& pLogConsumer
      );
};

class LogConsumerWriteToFile
{
public:
   LogConsumerWriteToFile(const std::string& logFilePath = GetDefaultPath());
   ~LogConsumerWriteToFile();
private:
   static const std::string GetDefaultPath();
   void Consumer( const std::string& message );
private:
   std::shared_ptr< std::function< void(const std::string&) > > m_logConsumer;
   std::ofstream m_file;
};

class LogConsumerConsole
{
public:
   LogConsumerConsole();
   ~LogConsumerConsole();
private:
   void Consumer( const std::string& message );
private:
   std::shared_ptr< std::function< void(const std::string&) > > m_logConsumer;
};

class LogScope
{
public:
   explicit LogScope(const char* const pName = nullptr);
   ~LogScope();
private:
   const char* const m_pName;
};

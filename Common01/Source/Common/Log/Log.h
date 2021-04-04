#pragma once

#if defined(_DEBUG)
   #define DSC_LOG
#endif

enum class LogTopic
{
   None = 0,
   Debug,
   Warning,
   Error,
   Scope,
   Render
};

#if defined(DSC_LOG)
   #define LOG_MESSAGE(format, ...) Log::AddMessage((int)LogTopic::None, format, ##__VA_ARGS__)
   #define LOG_MESSAGE_DEBUG(format, ...) Log::AddMessage((int)LogTopic::Debug, format, ##__VA_ARGS__)
   #define LOG_MESSAGE_WARNING(format, ...) Log::AddMessage((int)LogTopic::Warning, format, ##__VA_ARGS__)
   #define LOG_MESSAGE_ERROR(format, ...) Log::AddMessage((int)LogTopic::Error, format, ##__VA_ARGS__)
   #define LOG_MESSAGE_SCOPE(format, ...) Log::AddMessage((int)LogTopic::Scope, format, ##__VA_ARGS__)
   #define LOG_MESSAGE_RENDER(format, ...) Log::AddMessage((int)LogTopic::Render, format, ##__VA_ARGS__)
#else
   #define LOG_MESSAGE(format, ...) (void)0
   #define LOG_MESSAGE_DEBUG(format, ...) (void)0
   #define LOG_MESSAGE_WARNING(format, ...) (void)0
   #define LOG_MESSAGE_ERROR(format, ...) (void)0
   #define LOG_MESSAGE_SCOPE(format, ...) (void)0
   #define LOG_MESSAGE_RENDER(format, ...) (void)0
#endif

namespace Log
{
   void AddMessage(const int topic, const char* const pFormat, ... );

   void AddLogConsumer(
      const std::shared_ptr< std::function< void(const int, const std::string&) > >& pLogConsumer
      );
   void RemoveLogConsumer(
      const std::shared_ptr< std::function< void(const int, const std::string&) > >& pLogConsumer
      );
};

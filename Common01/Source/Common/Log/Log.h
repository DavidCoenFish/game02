#pragma once

#if defined(_DEBUG)
   #define DSC_LOG
#endif

/*
keep on seeing a chicken and egg for who gets started first between file system and log
some logs may want to write to file using file system
some logs may want to flush on every write and not have any external dependencies
some logs may only want to write at application end of life?
some logs may want to append a log file
some logs may only want to save out file for messages of certain topic
*/

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

class ILogConsumer;
namespace Log
{
   void AddMessage(const int topic, const char* const pFormat, ... );

   void AddLogConsumer(
      ILogConsumer& logConsumer
      );
   void RemoveLogConsumer(
      ILogConsumer& logConsumer
      );
};

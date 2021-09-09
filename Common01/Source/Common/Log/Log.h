#pragma once

#if defined(_DEBUG)
   #define DSC_LOG
#endif

#include "Common/Log/LogEnum.h"

/*
keep on seeing a chicken and egg for who gets started first between file system and log
some logs may want to write to file using file system
some logs may want to flush on every write and not have any external dependencies
some logs may only want to write at application end of life?
some logs may want to append a log file
some logs may only want to save out file for messages of certain topic
some logs want to emit to stdout/console/push

move to run time constant set of consumers, and Lob having scope. 
the consumers may need to queue messages till they are ready, but may not need runtime flexibility
advantage of not needing to lock the thread of consumers on every message sent
*/

#if defined(DSC_LOG)
   #define LOG_MESSAGE(format, ...) Log::AddMessage(LogTopic::None, format, ##__VA_ARGS__)
   #define LOG_MESSAGE_DEBUG(format, ...) Log::AddMessage(LogTopic::Debug, format, ##__VA_ARGS__)
   #define LOG_MESSAGE_WARNING(format, ...) Log::AddMessage(LogTopic::Warning, format, ##__VA_ARGS__)
   #define LOG_MESSAGE_ERROR(format, ...) Log::AddMessage(LogTopic::Error, format, ##__VA_ARGS__)
   #define LOG_MESSAGE_SCOPE(format, ...) Log::AddMessage(LogTopic::Scope, format, ##__VA_ARGS__)
   #define LOG_MESSAGE_FILESYSTEM(format, ...) Log::AddMessage(LogTopic::FileSystem, format, ##__VA_ARGS__)
   #define LOG_MESSAGE_UISYSTEM(format, ...) Log::AddMessage(LogTopic::UISystem, format, ##__VA_ARGS__)
   #define LOG_MESSAGE_RENDER(format, ...) Log::AddMessage(LogTopic::Render, format, ##__VA_ARGS__)
#else
   #define LOG_MESSAGE(format, ...) (void)0
   #define LOG_MESSAGE_DEBUG(format, ...) (void)0
   #define LOG_MESSAGE_WARNING(format, ...) (void)0
   #define LOG_MESSAGE_ERROR(format, ...) (void)0
   #define LOG_MESSAGE_SCOPE(format, ...) (void)0
   #define LOG_MESSAGE_FILESYSTEM(format, ...) (void)0
   #define LOG_MESSAGE_UISYSTEM(format, ...) (void)0
   #define LOG_MESSAGE_RENDER(format, ...) (void)0
#endif

class ILogConsumer;
class LogImplimentation;
class Log
{
public:
   static std::shared_ptr< Log > Factory(const std::vector< std::shared_ptr< ILogConsumer >>& arrayConsumer);
   Log(const std::vector< std::shared_ptr< ILogConsumer >>& arrayConsumer);
   ~Log();

   //any thread, asserts if outside scope of Log object
   static void AddMessage(const LogTopic topic, const char* const pFormat, ... );

private:
   std::unique_ptr< LogImplimentation > m_pImplimentation;

};

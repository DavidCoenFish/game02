#pragma once
#include "Common/Log/ILogConsumer.h"

//writes to disk, no dependency on FileSystem class
class LogConsumerWriteToFile: public ILogConsumer
{
public:
   LogConsumerWriteToFile(const std::string& logFilePath = GetDefaultPath());
   ~LogConsumerWriteToFile();
private:
   static const std::string GetDefaultPath();
   virtual void AddMessage(const LogTopic topic, const std::string& message ) override;
private:
   std::mutex m_fileLock;
   std::ofstream m_file;
};

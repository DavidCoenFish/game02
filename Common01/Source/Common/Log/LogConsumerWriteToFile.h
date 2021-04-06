#pragma once
#include "Common/Log/ILogConsumer.h"

class LogConsumerWriteToFile: public ILogConsumer
{
public:
   LogConsumerWriteToFile(const std::string& logFilePath = GetDefaultPath());
   ~LogConsumerWriteToFile();
private:
   static const std::string GetDefaultPath();
   virtual void AddMessage(const int topic, const std::string& message ) override;
private:
   std::ofstream m_file;
};

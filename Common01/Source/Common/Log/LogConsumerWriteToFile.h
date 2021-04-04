#pragma once
#include "Common/Log/ILogConsumer.h"

class LogConsumerWriteToFile: public ILogConsumer
{
public:
   LogConsumerWriteToFile(const std::string& logFilePath = GetDefaultPath());
   ~LogConsumerWriteToFile();
private:
   static const std::string GetDefaultPath();
   void Consumer(const int, const std::string& message );
private:
   std::shared_ptr< std::function< void(const int, const std::string&) > > m_logConsumer;
   std::ofstream m_file;
};

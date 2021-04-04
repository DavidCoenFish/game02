#include "CommonPCH.h"

#include "Common/Log/LogConsumerWriteToFile.h"
#include "Common/Log/Log.h"

LogConsumerWriteToFile::LogConsumerWriteToFile(const std::string& logFilePath)
{
   m_file.open(logFilePath.c_str(), std::ofstream::out);
   m_logConsumer = std::make_shared< std::function< void(const int, const std::string&) > >([=](const int topic, const std::string& message)
   {
      this->Consumer(topic, message);
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

void LogConsumerWriteToFile::Consumer(const int topic, const std::string& message )
{
   if (true == m_file.is_open())
   {
      m_file << topic << message.c_str();
   }
}


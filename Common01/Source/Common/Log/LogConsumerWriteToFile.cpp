#include "CommonPCH.h"

#include "Common/Log/LogConsumerWriteToFile.h"
#include "Common/Log/Log.h"

LogConsumerWriteToFile::LogConsumerWriteToFile(const std::string& logFilePath)
{
   m_file.open(logFilePath.c_str(), std::ofstream::out);
   Log::AddLogConsumer(*this);
}

LogConsumerWriteToFile::~LogConsumerWriteToFile()
{
   Log::RemoveLogConsumer(*this);
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

void LogConsumerWriteToFile::AddMessage(const int topic, const std::string& message )
{
   if (true == m_file.is_open())
   {
      m_file << topic << message.c_str();
   }
}


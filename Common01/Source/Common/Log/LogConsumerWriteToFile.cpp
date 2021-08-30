#include "CommonPCH.h"

#include "Common/Log/LogConsumerWriteToFile.h"
#include "Common/Log/Log.h"

LogConsumerWriteToFile::LogConsumerWriteToFile(const std::string& logFilePath)
{
   m_file.open(logFilePath.c_str(), std::ofstream::out);
}

LogConsumerWriteToFile::~LogConsumerWriteToFile()
{
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

void LogConsumerWriteToFile::AddMessage(const LogTopic topic, const std::string& message )
{
   if (true == m_file.is_open())
   {
      m_file << (int)topic << message.c_str();
   }
}


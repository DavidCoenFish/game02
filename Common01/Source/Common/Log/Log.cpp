#include "CommonPCH.h"

#include "Common/Log/Log.h"
#include "Common/Log/ILogConsumer.h"
#include "Common/Util/WorkerTask.h"
#include "Common/Macro.h"

static std::atomic<LogImplimentation*> s_singleton = nullptr;

//didn't want threadwrapper into global scope 
class LogImplimentation
{
public:
   LogImplimentation(const std::vector< std::shared_ptr< ILogConsumer > >& logConsumers);
   ~LogImplimentation();
   void AddMessage(const LogTopic topic, const std::string& message);

private:
   void DoWork();

private:
   std::shared_ptr<WorkerTask> m_workerThread;

   std::list< std::pair< LogTopic, std::string > > m_listMessages;
   std::mutex m_listMessagesMutex;

   std::vector< std::shared_ptr< ILogConsumer > > m_logConsumers;

};

LogImplimentation::LogImplimentation(const std::vector< std::shared_ptr< ILogConsumer > >& logConsumers)
   : m_logConsumers(logConsumers)
{
   DSC_ASSERT(nullptr == s_singleton);
   s_singleton = this;
   m_workerThread = WorkerTask::Factory([=](){
      DoWork();
   });
}

LogImplimentation::~LogImplimentation()
{
   m_workerThread = nullptr;
   DSC_ASSERT(nullptr != s_singleton);
   s_singleton = nullptr;
}

void LogImplimentation::AddMessage(const LogTopic topic, const std::string& message)
{
   {
      std::lock_guard< std::mutex > lock(m_listMessagesMutex);
      m_listMessages.push_back(std::pair< LogTopic, std::string >(topic, message));
   }

   if (nullptr != m_workerThread)
   {
      m_workerThread->SignalWorkToDo();
   }
   return;
}

void LogImplimentation::DoWork()
{
   while (true)
   {
      std::pair< LogTopic, std::string > messagePair;
      {
         std::lock_guard< std::mutex > lock(m_listMessagesMutex);
         if (true == m_listMessages.empty())
         {
            return;
         }
         messagePair = m_listMessages.front();
         m_listMessages.pop_front();
      }

      for (auto pIter : m_logConsumers)
      {
         pIter->AddMessage(messagePair.first, messagePair.second);
      }
   }

   return;
}

std::shared_ptr< Log > Log::Factory(const std::vector< std::shared_ptr< ILogConsumer >>& arrayConsumer)
{
   return std::make_shared< Log >(arrayConsumer);
}
Log::Log(const std::vector< std::shared_ptr< ILogConsumer >>& arrayConsumer)
{
   m_pImplimentation = std::make_unique< LogImplimentation >(arrayConsumer);
}

Log::~Log()
{
   m_pImplimentation = nullptr;
}


//https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf/8098080
void Log::AddMessage(const LogTopic topic, const char* const format, ... )
{
   // initialize use of the variable argument array
   va_list vaArgs;
   va_start(vaArgs, format);

   // reliably acquire the size
   // from a copy of the variable argument array
   // and a functionally reliable call to mock the formatting
   va_list vaArgsCopy;
   va_copy(vaArgsCopy, vaArgs);
   const int iLen = std::vsnprintf(NULL, 0, format, vaArgsCopy);
   va_end(vaArgsCopy);

   // return a formatted string without risking memory mismanagement
   // and without assuming any compiler or platform specific behavior
   std::vector<char> zc(iLen + 1);
   std::vsnprintf(zc.data(), zc.size(), format, vaArgs);
   va_end(vaArgs);
   std::string message(zc.data(), iLen); 

   auto pImple = s_singleton.load();
   DSC_ASSERT(pImple);
   if (nullptr != pImple)
   {
      pImple->AddMessage(topic, message);
   }
}

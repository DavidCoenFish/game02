#include "CommonPCH.h"

#include "Common/Log/Log.h"
#include "Common/Log/ILogConsumer.h"
#include "Common/Util/ThreadWrapper.h"
#include "Common/Macro.h"

static std::atomic<LogImplimentation*> s_singleton = nullptr;

//didn't want threadwrapper into global scope 
class LogImplimentation
{
public:
   LogImplimentation();
   ~LogImplimentation();
   void AddMessage(const LogTopic topic, const std::string& message);
   void AddLogConsumer(
      const std::shared_ptr< ILogConsumer >& pLogConsumer
      );
   void RemoveLogConsumer(
      const std::shared_ptr< ILogConsumer >& pLogConsumer
      );

private:
   void DoWork();

private:
   std::shared_ptr<ThreadWrapper<void()>> m_workerThread;

   std::list< std::pair< LogTopic, std::string > > m_listMessages;
   std::mutex m_listMessagesMutex;

   std::vector< std::shared_ptr< ILogConsumer > > m_logConsumers;
   std::mutex m_logConsumersMutex;

};

LogImplimentation::LogImplimentation()
{
   DSC_ASSERT(nullptr == s_singleton);
   s_singleton = this;
   m_workerThread = ThreadWrapper<void()>::Factory([=](){
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

      {
         std::lock_guard< std::mutex > lock(m_logConsumersMutex);
         for (auto pIter : m_logConsumers)
         {
            pIter->AddMessage(messagePair.first, messagePair.second);
         }
      }
   }

   return;
}

void LogImplimentation::AddLogConsumer(
   const std::shared_ptr< ILogConsumer >& pLogConsumer
   )
{
   m_logConsumers.push_back(pLogConsumer);
}
void LogImplimentation::RemoveLogConsumer(
   const std::shared_ptr< ILogConsumer >& pLogConsumer
   )
{
   m_logConsumers.erase(std::remove(m_logConsumers.begin(), m_logConsumers.end(), pLogConsumer), m_logConsumers.end());
}

std::shared_ptr< Log > Log::Factory()
{
   return std::make_shared< Log >();
}
Log::Log()
{
   m_pImplimentation = std::make_unique< LogImplimentation >();
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

void Log::AddLogConsumer(
   const std::shared_ptr< ILogConsumer >& pLogConsumer
   )
{
   auto pImple = s_singleton.load();
   DSC_ASSERT(pImple);
   if (nullptr != pImple)
   {
      pImple->AddLogConsumer(pLogConsumer);
   }
}

void Log::RemoveLogConsumer(
   const std::shared_ptr< ILogConsumer >& pLogConsumer
   )
{
   auto pImple = s_singleton.load();
   DSC_ASSERT(pImple);
   if (nullptr != pImple)
   {
      pImple->RemoveLogConsumer(pLogConsumer);
   }
}

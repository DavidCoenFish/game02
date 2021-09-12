#include "CommonPCH.h"

#include "Common/Log/Log.h"
#include "Common/Log/ILogConsumer.h"
#include "Common/Worker/WorkerTask.h"
#include "Common/Macro.h"
#include "Common/Util/Utf8.h"

static std::atomic<LogImplimentation*> s_singleton = nullptr;
static std::list< std::pair< LogTopic, std::string > > s_listMessages;
static std::mutex s_listMessagesMutex;

class LogImplimentation
{
public:
   LogImplimentation(const std::vector< std::shared_ptr< ILogConsumer > >& logConsumers);
   ~LogImplimentation();
   const bool AcceptsTopic(const LogTopic topic);
   void AddMessage(const LogTopic topic, const std::string& message);

private:
   void DoWork();

private:
   std::shared_ptr<WorkerTask> m_workerThread;

   std::list< std::pair< LogTopic, std::string > > m_listMessages;
   std::mutex m_listMessagesMutex;

   //ownership
   std::vector< std::shared_ptr< ILogConsumer > > m_logConsumers;

   //cache the consumers for each topic so we don't have to keep on filtering
   std::vector< ILogConsumer*> m_topicLogConsumers[(unsigned int)LogTopic::Count];
};

LogImplimentation::LogImplimentation(const std::vector< std::shared_ptr< ILogConsumer > >& logConsumers)
   : m_logConsumers(logConsumers)
{
   DSC_ASSERT(nullptr == s_singleton);
   s_singleton = this;
   m_workerThread = WorkerTask::Factory([=](){
      DoWork();
   });
   //UpdateTopicLogConsumers();
   for (int index = 0; index < (int)LogTopic::Count; ++index)
   {
      for (auto iter : logConsumers)
      {
         if (true == iter->AcceptsTopic((LogTopic)index))
         {
            m_topicLogConsumers[index].push_back(iter.get());
         }
      }
   }
}

LogImplimentation::~LogImplimentation()
{
   m_workerThread = nullptr;
   DSC_ASSERT(nullptr != s_singleton);
   s_singleton = nullptr;
}

const bool LogImplimentation::AcceptsTopic(const LogTopic topic)
{
   return (0 != m_topicLogConsumers[(unsigned int)topic].size());
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

      for (auto pIter : m_topicLogConsumers[(unsigned int)messagePair.first])
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

static const std::string FormatString(const char* const pFormat, va_list vaArgs)
{
   // reliably acquire the size
   // from a copy of the variable argument array
   // and a functionally reliable call to mock the formatting
   va_list vaArgsCopy;
   va_copy(vaArgsCopy, vaArgs);
   const int iLen = std::vsnprintf(NULL, 0, pFormat, vaArgsCopy);
   va_end(vaArgsCopy);

   // return a formatted string without risking memory mismanagement
   // and without assuming any compiler or platform specific behavior
   std::vector<char> zc(iLen + 1);
   std::vsnprintf(zc.data(), zc.size(), pFormat, vaArgs);

   std::string message(zc.data(), iLen); 

   return message;
}

//https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf/8098080
void Log::AddMessage(const LogTopic topic, const char* const pFormat, ... )
{
   auto pImple = s_singleton.load();
   // you know what, if the log doesn't exist, that may be intentional
   //DSC_ASSERT(pImple);
   bool bDumpToMemory = false;
   if (nullptr == pImple)
   {
      bDumpToMemory = true;
   }

   if ((nullptr != pImple) && (false == pImple->AcceptsTopic(topic)))
   {
      return;
   }

   va_list vaArgs;
   va_start(vaArgs, pFormat);
   const std::string message = FormatString(pFormat, vaArgs);
   va_end(vaArgs);

   if (false == bDumpToMemory)
   {
      pImple->AddMessage(topic, message);
   }
   else
   {
      //save log messages to memory if there is not Log class?
      //todo: assert on shutdown if never consumed?
      {
         std::lock_guard< std::mutex > lock(s_listMessagesMutex);
         s_listMessages.push_back(std::pair< LogTopic, std::string >(topic, message));
      }
      // we could log the fact that we are logging outside log scope? seems redundant though
      //if (topic != LogTopic::Log)
      //{
      //   AddMessage(LogTopic::Log, "Warning: topic:[%d] is trying to add messages to log outside log scope");
      //}
   }
}

void Log::AddConsole(const char* const pFormat, ... )
{
   va_list vaArgs;
   va_start(vaArgs, pFormat);
   std::string message = FormatString(pFormat, vaArgs);
   va_end(vaArgs);

   message += "\n";
   OutputDebugStringW(Utf8::Utf8ToUtf16(message).c_str());
   //OutputDebugStringW(L"\n");
}


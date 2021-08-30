#include "CommonPCH.h"

#include "Common/Log/Log.h"
#include "Common/Log/ILogConsumer.h"
#include "Common/Util/ThreadWrapper.h"

static std::atomic<class LogImple*> s_singleton = nullptr;

//didn't want threadwrapper into global scope 
class LogImplimentation
{
public:

   //const int topic, const std::string& message

private:
   std::shared_ptr<ThreadWrapper<void()>> m_workerThread;

   std::list< std::pair< LogTopic, std::string > > m_listMessages;
   std::mutex m_listMessagesMutex;

   std::vector< std::shared_ptr< ILogConsumer > > m_logConsumers;
   std::mutex m_logConsumersMutex;

};




//this doesn't gain much in this case, is a patteren to lazy init static vectors
static std::vector< std::shared_ptr< ILogConsumer > >& GetLogConsumerArray()
{
   static std::vector< std::shared_ptr< ILogConsumer > > s_logConsumerArray;
   return s_logConsumerArray;
}

static void AddMessageInternal(const int topic, const std::string& message)
{
   const auto& functorArray = GetLogConsumerArray();
   for (auto pIter : functorArray)
   {
      pIter->AddMessage(topic, message);
   }
   return;
}

//https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf/8098080
void Log::AddMessage(const int topic, const char* const format, ... )
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
   AddMessageInternal(topic, message);
}

void Log::AddLogConsumer(
   const std::shared_ptr< ILogConsumer >& pLogConsumer
   )
{
   auto& functorArray = GetLogConsumerArray();
   functorArray.push_back(pLogConsumer);
}
void Log::RemoveLogConsumer(
   const std::shared_ptr< ILogConsumer >& pLogConsumer
   )
{
   auto& functorArray = GetLogConsumerArray();
   functorArray.erase(std::remove(functorArray.begin(), functorArray.end(), pLogConsumer), functorArray.end());
}

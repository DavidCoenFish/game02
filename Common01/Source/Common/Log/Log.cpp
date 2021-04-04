#include "CommonPCH.h"

#include "Common/Log/Log.h"

//this doesn't gain much in this case, is a patteren to lazy init static vectors
static std::vector< std::shared_ptr< std::function< void(const int, const std::string&) > > >& GetFunctorArray()
{
   static std::vector< std::shared_ptr< std::function< void(const int, const std::string&) > > > s_functorArray;
   return s_functorArray;
}

static void AddMessageInternal(const int topic, const std::string& message)
{
   const auto& functorArray = GetFunctorArray();
   for (const auto& iter : functorArray)
   {
      (*iter)(topic, message);
   }
   return;
}

void Log::AddMessage(const int topic, const char* const format, ... )
{
   va_list argptr;
   va_start(argptr, format);
   int size = vsnprintf(nullptr, 0, format, argptr) + 1;
   if (0 < size)
   {
      std::string message;
      message.reserve(size);
      vsnprintf_s(message.data(), size, _TRUNCATE, format, argptr);
      AddMessageInternal(topic, message);
   }
   va_end(argptr);
}

void Log::AddLogConsumer(
   const std::shared_ptr< std::function< void(const int, const std::string&) > >& pLogConsumer
   )
{
   auto& functorArray = GetFunctorArray();
   functorArray.push_back(pLogConsumer);
}

void Log::RemoveLogConsumer(
   const std::shared_ptr< std::function< void(const int, const std::string&) > >& pLogConsumer
   )
{
   auto& functorArray = GetFunctorArray();
   auto iter = std::remove(functorArray.begin(), functorArray.end(), pLogConsumer);
   iter;
}


#include "CommonPCH.h"

#include "Common/Log/ILogConsumer.h"
#include "Common/Log/Log.h"

ILogConsumer::ILogConsumer()
{
   Log::AddLogConsumer(*this);
}


ILogConsumer::~ILogConsumer()
{
   Log::RemoveLogConsumer(*this);
}

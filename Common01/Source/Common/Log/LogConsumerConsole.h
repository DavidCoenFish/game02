#pragma once
#include "Common/Log/ILogConsumer.h"

class LogConsumerConsole : public ILogConsumer
{
public:
   LogConsumerConsole();
   ~LogConsumerConsole();
private:
   void Consumer(const int, const std::string& message );
private:
   std::shared_ptr< std::function< void(const int, const std::string&) > > m_logConsumer;
};

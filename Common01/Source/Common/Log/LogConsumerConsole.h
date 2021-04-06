#pragma once
#include "Common/Log/ILogConsumer.h"

class LogConsumerConsole : public ILogConsumer
{
public:
   LogConsumerConsole();
   ~LogConsumerConsole();
private:
   virtual void AddMessage(const int topic, const std::string& message ) override;

};

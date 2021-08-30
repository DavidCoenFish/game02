#pragma once

#include "Common/Log/LogEnum.h"

class ILogConsumer
{
public:
   virtual ~ILogConsumer();

   virtual void AddMessage(const LogTopic topic, const std::string& message ) = 0;

};

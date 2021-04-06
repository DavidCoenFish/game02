#pragma once

class ILogConsumer
{
public:
   ILogConsumer();
   virtual ~ILogConsumer();

   virtual void AddMessage(const int topic, const std::string& message ) = 0;

};

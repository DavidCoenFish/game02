#pragma once

class ILogConsumer
{
public:
   virtual ~ILogConsumer();

   virtual void AddMessage(const int topic, const std::string& message ) = 0;

};

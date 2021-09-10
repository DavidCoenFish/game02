#include "UnitTestPCH.h"

#include "Common/Log/Log.h"
#include "Common/Log/ILogConsumer.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

namespace CommonLog
{
   class LogConsumerTest : public ILogConsumer
   {
   public:
      const std::string GetData()
      {
         std::lock_guard lock(m_dataMutex);
         return m_data;
      }
   private:
      virtual void AddMessage(const LogTopic topic, const std::string& message ) override
      {
         std::string text = std::to_string((int)topic) + std::string(":") + message + "\n";
         std::lock_guard lock(m_dataMutex);
         m_data += text;
      }
      virtual const bool AcceptsTopic(const LogTopic) override
      {
         return true;
      }

   private:
      std::mutex m_dataMutex;
      std::string m_data;
   };

   TEST_CLASS(Sanity)
   {
   public: 
      TEST_METHOD(Simple)
      {
         {
            auto pConsumer = std::make_shared<LogConsumerTest>();
            {
               auto pLog = Log::Factory(std::vector< std::shared_ptr< ILogConsumer >>({ pConsumer }));
               Log::AddMessage(LogTopic(3), "Hello %s", "World");
            }
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual("3:Hello World\n", pConsumer->GetData().c_str());
         }
         {
            auto pConsumer = std::make_shared<LogConsumerTest>();
            {
               auto pLog = Log::Factory(std::vector< std::shared_ptr< ILogConsumer >>({ pConsumer }));
               Log::AddMessage(LogTopic(3), "Hello %s", "World");
               Log::AddMessage(LogTopic(4), "Goodbye %s", "Pain");
            }
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual("3:Hello World\n4:Goodbye Pain\n", pConsumer->GetData().c_str());
         }
      }
   };
}

#include "UnitTestPCH.h"

#include "Common/Log/Log.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

namespace CommonLog
{
   class LogConsumerTest
   {
   public:
      LogConsumerTest()
      {
         m_logConsumer = std::make_shared< std::function< void(const int, const std::string&) > >([=](const int topic, const std::string& message)
         {
            this->Consumer(topic, message);
         });
         Log::AddLogConsumer(m_logConsumer);
      }
      ~LogConsumerTest()
      {
         Log::RemoveLogConsumer(m_logConsumer);
      }
      const std::string& GetData()
      {
         return m_data;
      }
   private:
      void Consumer(const int topic, const std::string& message )
      {
         std::string text = std::to_string(topic) + std::string(":") + message + "\n";
         m_data += text;
      }
   private:
      std::shared_ptr< std::function< void(const int, const std::string&) > > m_logConsumer;
      std::string m_data;
   };

   TEST_CLASS(Sanity)
   {
   public: 
      TEST_METHOD(Simple)
      {
         {
            LogConsumerTest logConsumerTest;
            Log::AddMessage(3, "Hello %s", "World");
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual("3:Hello World\n", logConsumerTest.GetData().c_str());
         }
         {
            LogConsumerTest logConsumerTest;
            Log::AddMessage(3, "Hello %s", "World");
            Log::AddMessage(4, "Goodbye %s", "Pain");
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual("3:Hello World\n4:Goodbye Pain\n", logConsumerTest.GetData().c_str());
         }
      }
   };
}

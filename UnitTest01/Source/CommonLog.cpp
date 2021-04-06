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
      const std::string& GetData()
      {
         return m_data;
      }
   private:
      virtual void AddMessage(const int topic, const std::string& message ) override
      {
         std::string text = std::to_string(topic) + std::string(":") + message + "\n";
         m_data += text;
      }
   private:
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

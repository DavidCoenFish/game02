#include "UnitTestPCH.h"

#include "Common/Util/ThreadWrapper.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

namespace CommonUtilThreadWrapper
{
   TEST_CLASS(Basic)
   {

   public:
 
      TEST_METHOD(Basic0)
      {
         std::mutex countMutex;
         int count = 0;
         {
            auto pThreadWrapper = ThreadWrapper<void(),void>::Factory([&]()
            {
               std::lock_guard< std::mutex > lock(countMutex);
               //std::this_thread::sleep_for(std::chrono::nanoseconds(10));
               count += 1;
            });
            pThreadWrapper->SignalWorkToDo();
            std::this_thread::sleep_for(std::chrono::nanoseconds(10));
         }
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(1, count );
      }

   };
}

#include "UnitTestPCH.h"

#include "Common/Util/ThreadCollection.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

namespace CommonUtilThreadCollection
{
#if 0
   TEST_CLASS(Basic)
   {

   public:
 
      TEST_METHOD(TestCallback)
      {
         ThreadCollection<4> threadCollection;
         std::mutex countMutex;
         int count = 0;

         auto task = [&]()
         {
            count += 1;
         };
         std::vector< std::function< void() > >taskArray;
         taskArray.push_back(task);
         taskArray.push_back(task);
         taskArray.push_back(task);
         taskArray.push_back(task);

         threadCollection.SetActiveWorkFinishedCallback([&]()
         {
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(4, count);
         });
         threadCollection.AddWork(taskArray);
      }

   };
#endif //0
}

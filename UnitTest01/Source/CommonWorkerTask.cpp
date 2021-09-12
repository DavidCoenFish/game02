#include "UnitTestPCH.h"
#include "Common/Worker/WorkerTask.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

namespace CommonUtilWorkerTask
{
   TEST_CLASS(Basic)
   {

   public:
 
      TEST_METHOD(DoesDtorWaitForTask)
      {
         std::mutex countMutex;
         int count = 0;
         {
            auto pWorkerTask = WorkerTask::Factory([&]()
            {
               std::this_thread::sleep_for(std::chrono::milliseconds(100));
               //std::this_thread::sleep_for(std::chrono::seconds(10));
               {
                  std::lock_guard< std::mutex > lock(countMutex);
                  count += 1;
               }
            });
            pWorkerTask->SignalWorkToDo();
         }
         {
            std::lock_guard< std::mutex > lock(countMutex);
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(1, count );
         }
      }

#if 0
      TEST_METHOD(DoesDtorWaitForTask100)
      {
         for (int index = 0; index < 100; ++index)
         {
            Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(std::to_string(index).c_str());

            std::mutex countMutex;
            int count = 0;
            {
               auto pWorkerTask = WorkerTask::Factory([&]()
               {
                  std::lock_guard< std::mutex > lock(countMutex);
                  count += 1;
               });
               pWorkerTask->SignalWorkToDo();
            }
            {
               std::lock_guard< std::mutex > lock(countMutex);
               Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(1, count );
            }
         }
      }
#endif

   };
}

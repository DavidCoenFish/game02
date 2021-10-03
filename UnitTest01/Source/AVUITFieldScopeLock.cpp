#include "UnitTestPCH.h"
#include "Common/Worker/WorkerTask.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

#if 1
   #define LOGGER(message) Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(message)
#else
   #define LOGGER(message) (void)0
#endif

//std::scoped_lock;

// snippit of code
namespace AVUI
{
class Object
{
    template<class T>
    friend class TFieldScopeLock;
public:
   Object() : m_lockField(0), m_someDummyTestValueToTryToStopCodeBeingOptimisedAway(0) {}
   void DoThing();
private:
    volatile long m_lockField;
    //mutable std::mutex m_lockField;
public:
   int m_someDummyTestValueToTryToStopCodeBeingOptimisedAway;
};

class Object2
{
    template<class T>
    friend class TFieldScopeLock2;
public:
   Object2() : m_lockField(0), m_someDummyTestValueToTryToStopCodeBeingOptimisedAway(0) {}
   void DoThing();
private:
    volatile long m_lockField;
    mutable std::mutex m_mutex;
public:
   int m_someDummyTestValueToTryToStopCodeBeingOptimisedAway;
};

class HostPlatform
{
public:
   static void HostPlatform::YieldCurrentThread()
   {
       SwitchToThread();
   }
};
#define AVUI_INTERLOCKED_COMPARE_EXCHANGE(a, b, c) _InterlockedCompareExchange(&(a), (b), (c))

template<class T>
class TFieldScopeLock
{
public:
    TFieldScopeLock(T* pT) : m_pT(pT)
    {
        //LOGGER("lock ctor");
        while(AVUI_INTERLOCKED_COMPARE_EXCHANGE(pT->m_lockField, 1, 0))
        {
            //LOGGER("lock spin");
            HostPlatform::YieldCurrentThread();
        }
    }
    ~TFieldScopeLock()
    {
        //LOGGER("lock dtor");
        m_pT->m_lockField = 0;
    }
private:
    T* m_pT;
};

#if 1
template<class T>
class TFieldScopeLock2 : std::scoped_lock<std::mutex>
{
public:
    TFieldScopeLock2(T* pT) : std::scoped_lock<std::mutex>( pT->m_mutex )
    {
       //nop
    }
};
#else
template<class T>
class TFieldScopeLock2
{
public:
    TFieldScopeLock2(T* pT) : m_pT(pT)
    {
        while(AVUI_INTERLOCKED_COMPARE_EXCHANGE(pT->m_lockField, 1, 0))
        {
            HostPlatform::YieldCurrentThread();
        }
    }
    ~TFieldScopeLock2()
    {
       _InterlockedExchange(&(m_pT->m_lockField), 0L);
    }
private:
    T* m_pT;
};
#endif

void Object::DoThing()
{
   TFieldScopeLock<Object> scopeLock(this);
   //lets add some contention
   //std::this_thread::sleep_for(std::chrono::milliseconds(15));
   //LOGGER("DoThing");
   m_someDummyTestValueToTryToStopCodeBeingOptimisedAway += 1;
}
void Object2::DoThing()
{
   TFieldScopeLock2<Object2> scopeLock(this);
   //lets add some contention
   //std::this_thread::sleep_for(std::chrono::milliseconds(15));
   //LOGGER("DoThing");
   m_someDummyTestValueToTryToStopCodeBeingOptimisedAway += 1;
}


};

namespace AVUITFieldScopeLock
{
   TEST_CLASS(Repo)
   {
   public: 
      TEST_METHOD(TryToRecreateDeadlock)
      {
         LOGGER("about to start making threads and doing things");
         AVUI::Object object;
         {
            std::vector< std::shared_ptr<WorkerTask> > arrayWorkerThreads;
            for (int index = 0; index < 10; ++index)
            {
               auto pTask = WorkerTask::Factory([&]()
               {
                  for (int index = 0; index < 100; ++index)
                  {
                     object.DoThing();
                  }
               });
               //pTask->SignalWorkToDo();
               arrayWorkerThreads.push_back(pTask);
            }
            for( const auto& iter: arrayWorkerThreads)
            {
               iter->SignalWorkToDo();
            }
         }
         LOGGER(std::to_string(object.m_someDummyTestValueToTryToStopCodeBeingOptimisedAway).c_str());
      }
   };
#if 0
   TEST_CLASS(Fix)
   {
   public: 
      TEST_METHOD(TryToFixDeadlock)
      {
         LOGGER("about to start making threads and doing things");
         AVUI::Object2 object;
         {
            std::vector< std::shared_ptr<WorkerTask> > arrayWorkerThreads;
            for (int index = 0; index < 100; ++index)
            {
               auto pTask = WorkerTask::Factory([&]()
               {
                  for (int index = 0; index < 10000; ++index)
                  {
                     object.DoThing();
                  }
               });
               //pTask->SignalWorkToDo();
               arrayWorkerThreads.push_back(pTask);
            }
            for( const auto& iter: arrayWorkerThreads)
            {
               iter->SignalWorkToDo();
            }
         }
         LOGGER(std::to_string(object.m_someDummyTestValueToTryToStopCodeBeingOptimisedAway).c_str());
      }
   };
#endif
}

#include "CommonPCH.h"

#include "Common/Worker/WorkerTask.h"
#if 1
std::shared_ptr<WorkerTask> WorkerTask::Factory(const std::function<void(void)>& task)
{
   return std::make_shared<WorkerTask>(task);
}

WorkerTask::WorkerTask(const std::function<void(void)>& task) //const std::future<_Ty>& future)
: m_task(task)
, m_event(0)
, m_flagWork(0)
, m_flagDtor(false)
{
   m_event = CreateEvent( 
      NULL,   // default security attributes
      FALSE,  // auto-reset event object
      FALSE,  // initial state is nonsignaled
      NULL);  // unnamed object

   m_future = std::async(std::launch::async, [=](){
      DoWork();
   });
   return;
}

WorkerTask::~WorkerTask()
{
   m_flagDtor = true;
   if (nullptr != m_event)
   {
      SetEvent(m_event);
   }
   if (true == m_future.valid())
   {
      m_future.wait();
   }
   if (nullptr != m_event)
   {
      CloseHandle(m_event);
   }
   m_event = nullptr;
   return;
}

void WorkerTask::SignalWorkToDo()
{
   m_flagWork++;
   SetEvent(m_event);
   return;
}

void WorkerTask::DoWork()
{
   int flagWorkCache = 0;
   while (true)
   {
      WaitForSingleObject(m_event, INFINITE);

      const int flagWork = m_flagWork.load();
      if (flagWorkCache != flagWork)
      {
         flagWorkCache = flagWork;
         m_task();
      }

      if (true == m_flagDtor)
      {
         break;
      }
   }
   return;
}

#elif 0
std::shared_ptr<WorkerTask> WorkerTask::Factory(const std::function<void(void)>& task)
{
   return std::make_shared<WorkerTask>(task);
}

WorkerTask::WorkerTask(const std::function<void(void)>& task) //const std::future<_Ty>& future)
: m_task(task)
, m_flagWork(false)
, m_flagDtor(false)
{
   m_future = std::async(std::launch::async, [=](){
      DoWork();
   });
   return;
}

WorkerTask::~WorkerTask()
{
   {
      std::lock_guard<std::mutex> lock(m_conditionVariableMutex);
      m_flagDtor = true;
   }
   m_conditionVariable.notify_one();

   m_future.wait();
   return;
}

void WorkerTask::SignalWorkToDo()
{
   {
      std::lock_guard<std::mutex> lock(m_conditionVariableMutex);
      m_flagWork = true;
   }
   m_conditionVariable.notify_one();

   return;
}

void WorkerTask::DoWork()
{
   while (true)
   {
      std::unique_lock<std::mutex> lock(m_conditionVariableMutex);
      // this code block is to deal with m_flagDtor being set while we are not waiting
      if ((true == m_flagDtor) && (false == m_flagWork))
      {
         break;
      }
      //if we already have work, we don't need to wait
      if (false == m_flagWork)
      {
         m_conditionVariable.wait(lock); 
      }

      bool work = false;
      if (true == m_flagWork)
      {
         m_flagWork = false;
         work = true;
      }
      //trying to not have the scope of the lock over the task, to allow us not to block on signal of more work
      lock.unlock();

      if (true == work)
      {
         m_task();
      }

      if (true == m_flagDtor)
      {
         break;
      }
   }
   return;
}





#else
std::shared_ptr<WorkerTask> WorkerTask::Factory(const std::function<void(void)>& task)
{
   return std::make_shared<WorkerTask>(task);
}

WorkerTask::WorkerTask(const std::function<void(void)>& task) //const std::future<_Ty>& future)
: m_task(task)
, m_flagWork(false)
, m_flagDtor(false)
{
   m_event = CreateEvent( 
      NULL,   // default security attributes
      FALSE,  // auto-reset event object
      FALSE,  // initial state is nonsignaled
      NULL);  // unnamed object

   m_future = std::async(std::launch::async, [=](){
      DoWork();
   });
   return;
}

WorkerTask::~WorkerTask()
{
   {
      std::lock_guard< std::mutex > lock(m_flagMutex);
      m_flagDtor = true;
   }
   SetEvent(m_event);
   m_future.wait();
   //m_future.get();
   CloseHandle(m_event);
   m_event = nullptr;
   return;
}

void WorkerTask::SignalWorkToDo()
{
   {
      std::lock_guard< std::mutex > lock(m_flagMutex);
      m_flagWork = true;
   }
   SetEvent(m_event);
   return;
}

void WorkerTask::DoWork()
{
   while (true)
   {
      WaitForSingleObject(m_event, INFINITE);

      bool doWork = false;
      {
         std::lock_guard< std::mutex > lock(m_flagMutex);
         if (true == m_flagWork)
         {
            doWork = true;
            m_flagWork = false;
         }
      }
      if (true == doWork)
      {
         m_task();
      }
      {
         std::lock_guard< std::mutex > lock(m_flagMutex);
         if (true == m_flagDtor)
         {
            break;
         }
      }
   }
   return;
}

#endif
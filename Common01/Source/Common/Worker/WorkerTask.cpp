#include "CommonPCH.h"

#include "Common/Worker/WorkerTask.h"


#pragma once
/*
wanted a helper to manage getting a lambda into a thread, 
   then waiting for it if you wanted to destroy it,
   and handle telling it that there is work to be done. SignalWorkToDo()
*/

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

//WorkerTask::WorkerTask()
//: m_event(nullptr)
//, m_flagWork(false)
//, m_flagDtor(false)
//{
//   return;
//}

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


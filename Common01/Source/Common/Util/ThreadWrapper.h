#pragma once
/*
wanted a helper to manage getting a lambda into a thread, 
   then waiting for it if you wanted to destroy it,
   and handle telling it that there is work to be done. SignalWorkToDo()
*/

template <class _Ty, class _Return>
class ThreadWrapper
{
private:
   enum class EventType : unsigned int
   {
      ObjectDtor = 0,
      SignalWorkTodo,
      Count
   };
private:
   ThreadWrapper(const ThreadWrapper&);

public:
   static std::shared_ptr<ThreadWrapper> Factory(const std::function<_Ty>& task)
   {
      HANDLE eventObjectDtor = CreateEvent( 
         NULL,   // default security attributes
         FALSE,  // auto-reset event object
         FALSE,  // initial state is nonsignaled
         NULL);  // unnamed object
      HANDLE eventWorkTodo = CreateEvent( 
         NULL,   // default security attributes
         FALSE,  // auto-reset event object
         FALSE,  // initial state is nonsignaled
         NULL);  // unnamed object

      return std::make_shared<ThreadWrapper>(eventObjectDtor, eventWorkTodo, task);
   }

   ThreadWrapper(HANDLE eventObjectDtor, HANDLE eventWorkTodo, const std::function<_Ty>& task) //const std::future<_Ty>& future)
      : m_task(task)
   {
      m_event[(unsigned int)EventType::ObjectDtor] = eventObjectDtor;
      m_event[(unsigned int)EventType::SignalWorkTodo] = eventWorkTodo;
      m_future = std::async(std::launch::async, [=](){
         DoWork();
      });
      return;
   }

   ThreadWrapper()
   {
      return;
   }

   ~ThreadWrapper()
   {
      SetEvent(m_event[(unsigned int)EventType::ObjectDtor]);
      m_future.wait();
      CloseHandle(m_event[(unsigned int)EventType::ObjectDtor]);
      m_event[(unsigned int)EventType::ObjectDtor] = 0;
      CloseHandle(m_event[(unsigned int)EventType::SignalWorkTodo]);
      m_event[(unsigned int)EventType::SignalWorkTodo] = 0;
      return;
   }
   void SignalWorkToDo()
   {
      SetEvent(m_event[(unsigned int)EventType::SignalWorkTodo]);
      return;
   }

private:
   void DoWork()
   {
      while (true){
         const DWORD returnCode = WaitForMultipleObjects(
            (DWORD)EventType::Count,
            m_event,
            FALSE,
            INFINITE
            );
         switch(returnCode)
         {
         default:
            printf("thread return code [%d] not delt with\n", returnCode);
            return;
         case WAIT_OBJECT_0 + (unsigned int)EventType::ObjectDtor:
            return;
         case WAIT_OBJECT_0 + (unsigned int)EventType::SignalWorkTodo:
            break;
         }
         m_task();
      }
      return;
   }

private:
   HANDLE m_event[2];
   //std::future<_Ty> m_future;
   std::future<_Return> m_future;
   std::function<_Ty> m_task;

};

#pragma once
/*
   wanted a helper to manage getting a lambda into a thread, 
then waiting for it if you wanted to destroy it,
and handle telling it that there is work to be done. SignalWorkToDo()

not using multiple signals for the two states as was loosing info on [signal work, signal dtor, wait for multiple, returned dtor (missing work signal)]

*/
#if 1
class WorkerTask
{
private:
   WorkerTask(const WorkerTask&);

public:
   static std::shared_ptr<WorkerTask> Factory(const std::function<void(void)>& task);
   WorkerTask(const std::function<void(void)>& task);
   //WorkerTask();
   ~WorkerTask();
   void SignalWorkToDo();

private:
   void DoWork();

private:
   HANDLE m_event;
   std::future<void> m_future;
   std::function<void(void)> m_task;

   std::atomic_int m_flagWork;
   std::atomic_bool m_flagDtor;
};


#elif 0
class WorkerTask
{
private:
   WorkerTask(const WorkerTask&);

public:
   static std::shared_ptr<WorkerTask> Factory(const std::function<void(void)>& task);
   WorkerTask(const std::function<void(void)>& task);
   //WorkerTask();
   ~WorkerTask();
   void SignalWorkToDo();

private:
   void DoWork();

private:
   std::future<void> m_future;
   std::function<void(void)> m_task;

   std::mutex m_conditionVariableMutex;
   std::condition_variable m_conditionVariable;
   bool m_flagWork;
   bool m_flagDtor;

};

#else
class WorkerTask
{
private:
   WorkerTask(const WorkerTask&);

public:
   static std::shared_ptr<WorkerTask> Factory(const std::function<void(void)>& task);
   WorkerTask(const std::function<void(void)>& task);
   //WorkerTask();
   ~WorkerTask();
   void SignalWorkToDo();

private:
   void DoWork();

private:
   HANDLE m_event;
   std::future<void> m_future;
   std::function<void(void)> m_task;

   std::mutex m_flagMutex;
   bool m_flagWork;
   bool m_flagDtor;

};
#endif

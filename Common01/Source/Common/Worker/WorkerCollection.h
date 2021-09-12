#pragma once

/*
wanted a collection of worker threads that i could give a set of tasks to
for adding a callback for when all the tasks are done, see WorkerCollectionObserved
*/
#include "Common/Worker/WorkerTask.h"

template <int _ThreadCount>
class WorkerCollection
{
public:
   static std::shared_ptr<WorkerCollection> Factory()
   {
      return std::make_shared<WorkerCollection>();
   }

   WorkerCollection()
   {
      for (int index = 0; index < _ThreadCount; ++index)
      {
         m_workerThread[index] = std::make_shared<WorkerTask>([=](){DoWork();});
      }
      return;
   }

   ~WorkerCollection()
   {
      for (int index = 0; index < _ThreadCount; ++index)
      {
         m_workerThread[index] = nullptr;
      }
      return;
   }
   void AddWork(const std::vector< std::function<void()> >& workArray)
   {
      {
         std::lock_guard< std::mutex > lock(m_workArrayMutex);
         m_workArray.insert( m_workArray.end(), workArray.begin(), workArray.end() );
      }
      SignalWork();

      return;
   }

   void AddWork(const std::function<void()>& work)
   {
      {
         std::lock_guard< std::mutex > lock(m_workArrayMutex);
         m_workArray.push_back(work);
      }
      SignalWork();

      return;
   }

   void SetActiveWorkFinishedCallback(const std::function<void()>& callbackActiveWorkFinished)
   {
      std::lock_guard< std::mutex > lock(m_callbackActiveWorkFinishedMutex);
      m_callbackActiveWorkFinished.push_back(callbackActiveWorkFinished);
      return;
   }

private:
   void SignalWork()
   {
      for (int index = 0; index < _ThreadCount; ++index)
      {
         m_workerThread[index]->SignalWorkToDo();
      }
   }

   void DoWork()
   {
      std::function<void()> work;
      while (true)
      {
         {
            std::lock_guard< std::mutex > lock(m_workArrayMutex);
            if (false == m_workArray.empty())
            {
               work = m_workArray.front();
               m_workArray.pop_front();
            }
         }

         if (nullptr == work)
         {
            return;
         }

         work();
         work = nullptr;

         {
            std::lock_guard< std::mutex > lock(m_workArrayMutex);
            if (true == m_workArray.empty())
            {
               DoActiveWorkFinishedCallback();
            }
         }

      }
      //if there is any more on the array...? otherwise there is a risk of items never removed from list? use while loop instead
      //SignalWorkToDo();
   }

   void DoActiveWorkFinishedCallback()
   {
      std::vector< std::function<void()> > localCopy;
      {
         std::lock_guard< std::mutex > lock(m_callbackActiveWorkFinishedMutex);
         localCopy = m_callbackActiveWorkFinished;
         m_callbackActiveWorkFinished.clear();
      }
      for (auto iter: localCopy)
      {
         iter();
      }
   }

private:
   std::list< std::function<void(void)> > m_workArray;
   std::mutex m_workArrayMutex;

   std::shared_ptr<WorkerTask> m_workerThread[_ThreadCount];

   std::vector< std::function<void()> > m_callbackActiveWorkFinished;
   std::mutex m_callbackActiveWorkFinishedMutex;
};

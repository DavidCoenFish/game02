#pragma once

#if 0

/*
wanted a collection of worker threads that i could give a set of tasks to
for adding a callback for when all the tasks are done, see ThreadCollectionObserved
*/
#include "Common/Util/ThreadWrapper.h"

template <int _ThreadCount>
class ThreadCollection
{
public:
   static std::shared_ptr<ThreadCollection> Factory()
   {
      return std::make_shared<ThreadCollection>();
   }

   ThreadCollection()
      : m_activeThreadCount(0)
   {
      for (int index = 0; index < _ThreadCount; ++index)
      {
         m_workerThread[index] = std::make_shared<ThreadWrapper<void(),void>>([=](){DoWork();});
      }
      return;
   }

   ~ThreadCollection()
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
      //std::lock_guard< std::mutex > lock(m_workArrayMutex);
      //if (0 == m_activeThreadCount)
      //{
      //   callbackActiveWorkFinished();
      //   return;
      //}
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
      bool first = true;
      std::function<void()> work;
      while (true)
      {
         bool isEmpty = false;
         {
            std::lock_guard< std::mutex > lock(m_workArrayMutex);
            if (true == m_workArray.empty())
            {
               isEmpty = true;
            }
            else
            {
               work = m_workArray.front();
               m_workArray.pop_front();
            }
         }

         if (true == isEmpty)
         {
            if (false == first)
            {
               bool doCallbacks = false;
               {
                  std::lock_guard< std::mutex > lock(m_activeThreadCountMutex);
                  m_activeThreadCount -= 1;
                  doCallbacks = (0 == m_activeThreadCount);
               }
               if (0 == doCallbacks)
               {
                  DoActiveWorkFinishedCallback();
               }
            }

            return;
         }

         if (true == first)
         {
            first = false;
            std::lock_guard< std::mutex > lock(m_activeThreadCountMutex);
            m_activeThreadCount += 1;
         }

         work();
      }
      //if there is any more on the array...? otherwise there is a risk of items never removed from list? use while loop instead
      //SignalWorkToDo();
   }

   void DoActiveWorkFinishedCallback()
   {
      std::vector< std::function<void()> > copy;
      {
         std::lock_guard< std::mutex > lock(m_workArrayMutex);
         copy = m_callbackActiveWorkFinished;
         m_callbackActiveWorkFinished.clear();
      }
      for (auto iter: copy)
      {
         iter();
      }
   }

private:
   std::list< std::function<void()> > m_workArray;
   std::mutex m_workArrayMutex;

   int m_activeThreadCount;
   std::mutex m_activeThreadCountMutex;


   std::shared_ptr<ThreadWrapper<void(), void>> m_workerThread[_ThreadCount];
   //ThreadWrapper<void(), void> m_workerThread[_ThreadCount];

   std::vector< std::function<void()> > m_callbackActiveWorkFinished;
};

#endif //0

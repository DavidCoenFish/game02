//// ConsoleTest00.cpp : This file contains the 'main' function. Program execution begins and ends there.
//// https://thomastrapp.com/blog/signal-handler-for-multithreaded-c++/
//// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/signal?view=msvc-160

#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN
#define NOCOMM

#include <cstdlib>
#include <signal.h>
#include <stdio.h>
//#include <synchapi.h>
#include <future>
#include <functional>

#include <Windows.h>
//#include <WinBase.h>
//#include <winerror.h>

HANDLE gShutdownEvent = 0;
void SignalHandler(int signal)
{
   printf("SIGINT %d\n", signal);
   SetEvent(gShutdownEvent);
}

class WorkTask
{
private:
   enum class EventType : unsigned int
   {
      GlobalShutdown = 0,
      ObjectDtor,
      SignalTodo,
      Count
   };
public:
   //std::shared_ptr<WorkTask> Factory(const HANDLE shutdownEvent, const std::function<void>& task)
   WorkTask(const HANDLE shutdownEvent, const std::function<void()>& task)
      : mTask(task)
   {
      mEvent[(unsigned int)EventType::GlobalShutdown] = shutdownEvent;
      mEvent[(unsigned int)EventType::ObjectDtor] = CreateEvent( 
         NULL,   // default security attributes
         FALSE,  // auto-reset event object
         FALSE,  // initial state is nonsignaled
         NULL);  // unnamed object
      mEvent[(unsigned int)EventType::SignalTodo] = CreateEvent( 
         NULL,   // default security attributes
         FALSE,  // auto-reset event object
         FALSE,  // initial state is nonsignaled
         NULL);  // unnamed object
      mFuture = std::async(std::launch::async, [=](){
         DoWork();
      });
   }
private:
   //avoid anonomus function for profiling, so make a local method "DoWork"
   void DoWork()
   {
      while (true){
         const DWORD returnCode = WaitForMultipleObjects(
            (DWORD)EventType::Count,
            mEvent,
            FALSE,
            INFINITE
            );
         switch(returnCode)
         {
         default:
            printf("thread unknown return code %d\n", returnCode);
            break;
         case WAIT_OBJECT_0:
         case WAIT_OBJECT_0 + 1:
            return;
         case WAIT_OBJECT_0 + 2:
            break;
         }
         mTask();
      }
   }
public:
   ~WorkTask()
   {
      SetEvent(mEvent[(unsigned int)EventType::ObjectDtor]);
      mFuture.wait();
      CloseHandle(mEvent[(unsigned int)EventType::ObjectDtor]);
      mEvent[(unsigned int)EventType::ObjectDtor] = 0;
      CloseHandle(mEvent[(unsigned int)EventType::SignalTodo]);
      mEvent[(unsigned int)EventType::SignalTodo] = 0;
      return;
   }
   //not 100% sure what happens if we set the event before it is waited on
   void SignalWorkToDo()
   {
      SetEvent(mEvent[(unsigned int)EventType::SignalTodo]);
   }
private:
   HANDLE mEvent[3];
   std::future<void> mFuture;
   std::function<void()> mTask;
};

class WorkManager
{
public:
   WorkManager()
      : mShutdownEvent(0)
   {
      mShutdownEvent = CreateEvent( 
         NULL,   // default security attributes
         FALSE,  // auto-reset event object
         FALSE,  // initial state is nonsignaled
         NULL);  // unnamed object
   }
   ~WorkManager()
   {
      SetEvent(mShutdownEvent);
      //external things still using mShutdownEvent, so we don't call CloseHandle
   }
   std::shared_ptr< WorkTask > MakeTask(const std::function<void()>& function)
   {
      return std::make_shared< WorkTask >( mShutdownEvent, function );
   }

private:
   HANDLE mShutdownEvent;


};


int main()
{
   gShutdownEvent = CreateEvent( 
            NULL,   // default security attributes
            FALSE,  // auto-reset event object
            FALSE,  // initial state is nonsignaled
            NULL);  // unnamed object

   signal(SIGINT, SignalHandler);
   signal(SIGBREAK, SignalHandler);
   signal(SIGTERM, SignalHandler);
   signal(SIGABRT_COMPAT, SignalHandler);

   auto async = std::async(std::launch::async, [=](){
      while (true){
         auto returnCode = WaitForSingleObject(gShutdownEvent, 5000);
         switch(returnCode)
         {
         default:
            printf("thread unknown return code %d\n", returnCode);
            break;
         case WAIT_OBJECT_0:
            printf("thread WAIT_OBJECT_0\n");
            return;
         case WAIT_TIMEOUT:
            printf("thread WAIT_TIMEOUT\n");
            break;
         }
      }
   });

   printf("start wait\n");
   async.wait();
   printf("end wait\n");

   return EXIT_SUCCESS;
}


//
//#include <condition_variable>
//#include <cstdlib>
//#include <future>
//#include <iostream>
//#include <mutex>
//#include <thread>
//#include <vector>
//
//#include <signal.h>
//#include <synchapi.h>
//
//int main()
//{
//  // block signals in this thread and subsequently
//  // spawned threads
//  sigset_t sigset;
//  sigemptyset(&sigset);
//  sigaddset(&sigset, SIGINT);
//  sigaddset(&sigset, SIGTERM);
//  pthread_sigmask(SIG_BLOCK, &sigset, nullptr);
//
//  std::atomic<bool> shutdown_requested(false);
//  std::mutex cv_mutex;
//  std::condition_variable cv;
//
//  auto signal_handler = [&shutdown_requested, &cv, &sigset]() {
//    int signum = 0;
//    // wait until a signal is delivered:
//    sigwait(&sigset, &signum);
//    shutdown_requested.store(true);
//    // notify all waiting workers to check their predicate:
//    cv.notify_all();
//    return signum;
//  };
//
//  auto ft_signal_handler = std::async(std::launch::async, signal_handler);
//
//  auto worker = [&shutdown_requested, &cv_mutex, &cv]() {
//    while( shutdown_requested.load() == false )
//    {
//      std::unique_lock lock(cv_mutex);
//      cv.wait_for(
//          lock,
//          // wait for up to an hour
//          std::chrono::hours(1),
//          // when the condition variable is woken up and this predicate
//          // returns true, the wait is stopped:
//          [&shutdown_requested]() { return shutdown_requested.load(); });
//    }
//
//    return shutdown_requested.load();
//  };
//
//  // spawn a bunch of workers
//  std::vector<std::future<bool>> workers;
//  for( int i = 0; i < 10; ++i )
//    workers.push_back(std::async(std::launch::async, worker));
//
//  std::cout << "waiting for SIGTERM or SIGINT ([CTRL]+[c])...\n";
//
//  // wait for signal handler to complete
//  int signal = ft_signal_handler.get();
//  std::cout << "received signal " << signal << "\n";
//
//  // wait for workers
//  for( auto& future : workers )
//    std::cout << "worker observed shutdown request: "
//              << std::boolalpha
//              << future.get()
//              << "\n";
//
//  std::cout << "clean shutdown\n";
//
//  return EXIT_SUCCESS;
//}

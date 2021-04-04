#pragma once

class TaskHolder;
class IApplication;

const int WindowHelper(
   const std::shared_ptr<TaskHolder>& pTaskHolder,
   const std::function< IApplication*(HWND, const std::shared_ptr<TaskHolder>&) >& pApplicationFactory,
   HINSTANCE hInstance,
   const std::string& applicationName,
   const bool bFullScreen,
   const int defaultWidth,
   const int defaultHeight,
   const int nCmdShow
   );


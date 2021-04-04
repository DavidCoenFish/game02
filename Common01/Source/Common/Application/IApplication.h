#pragma once

class TaskHolder;

class IApplication
{
public:
   IApplication(HWND hWnd, const std::shared_ptr<TaskHolder>& pTaskHolder);
   virtual ~IApplication();

private:
   HWND m_hWnd; 
   std::shared_ptr<TaskHolder> m_pTaskHolder;

};
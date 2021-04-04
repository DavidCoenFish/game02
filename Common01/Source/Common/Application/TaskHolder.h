#pragma once

class TaskHolder
{
public:
   TaskHolder();
   void AddHwnd(HWND hWnd);
   void RemoveHwnd(HWND hWnd);
   const bool HasHwnd() const;

private:
   mutable std::mutex m_windowsMutex;
   std::vector<HWND> m_windows;

};
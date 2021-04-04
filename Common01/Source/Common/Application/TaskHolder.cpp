#include "CommonPCH.h"
#include "Common/Application/TaskHolder.h"

TaskHolder::TaskHolder()
{
   //nop
}
void TaskHolder::AddHwnd(HWND hWnd)
{
   std::lock_guard< std::mutex >lock(m_windowsMutex);
   m_windows.push_back(hWnd);
}

void TaskHolder::RemoveHwnd(HWND hWnd)
{
   std::lock_guard< std::mutex >lock(m_windowsMutex);
   m_windows.erase(std::remove(m_windows.begin(), m_windows.end(), hWnd), m_windows.end());
}

const bool TaskHolder::HasHwnd() const
{
   std::lock_guard< std::mutex >lock(m_windowsMutex);
   return (0 < m_windows.size());
}


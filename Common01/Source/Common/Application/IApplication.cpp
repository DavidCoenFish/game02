#include "CommonPCH.h"
#include "Common/Application/IApplication.h"
#include "Common/Application/TaskHolder.h"
#include "Common/Log/Log.h"

IApplication::IApplication(HWND hWnd, const std::shared_ptr<TaskHolder>& pTaskHolder)
   : m_hWnd(hWnd)
   , m_pTaskHolder(pTaskHolder)
{
   LOG_MESSAGE("IApplication ctor %p", this);
   if (nullptr != m_pTaskHolder)
   {
      m_pTaskHolder->AddHwnd(m_hWnd);
   }
}

IApplication::~IApplication()
{
   LOG_MESSAGE("IApplication dtor %p", this);

   if (nullptr != m_pTaskHolder)
   {
      m_pTaskHolder->RemoveHwnd(m_hWnd);
   }
}

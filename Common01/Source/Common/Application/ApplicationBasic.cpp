#include "CommonPCH.h"
#include "Common/Application/ApplicationBasic.h"
#include "Common/Log/Log.h"
#include "Common/DrawSystem/DrawSystem.h"

ApplicationBasic::ApplicationBasic(const IApplicationParam& applicationParam)
   : IApplication(applicationParam)
{
   LOG_MESSAGE("ApplicationBasic ctor %p", this);
   m_pDrawSystem = std::make_unique< DrawSystem>(applicationParam.m_hWnd);
}

ApplicationBasic::~ApplicationBasic()
{
   LOG_MESSAGE("ApplicationBasic dtor %p", this);
}

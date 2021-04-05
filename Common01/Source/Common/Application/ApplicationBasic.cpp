#include "CommonPCH.h"
#include "Common/Application/ApplicationBasic.h"
#include "Common/Log/Log.h"

ApplicationBasic::ApplicationBasic(const IApplicationParam& applicationParam)
   : IApplication(applicationParam)
{
   LOG_MESSAGE("ApplicationBasic ctor %p", this);
}

ApplicationBasic::~ApplicationBasic()
{
   LOG_MESSAGE("ApplicationBasic dtor %p", this);
}

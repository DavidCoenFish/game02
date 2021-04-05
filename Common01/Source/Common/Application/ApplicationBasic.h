#pragma once

#include "Common/Application/IApplication.h"
class ApplicationHolder;
class DrawSystem;

class ApplicationBasic : public IApplication
{
public:
   ApplicationBasic(const IApplicationParam& applicationParam);
   virtual ~ApplicationBasic();

private:
   std::unique_ptr< DrawSystem > m_pDrawSystem;
};
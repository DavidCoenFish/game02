#pragma once

#include "Common/Application/IApplication.h"
class ApplicationHolder;
class DrawSystem;

class ApplicationBasic : public IApplication
{
   typedef IApplication BaseType;
public:
   ApplicationBasic(const IApplicationParam& applicationParam);
   virtual ~ApplicationBasic();

   virtual void Update() override;

private:
   std::unique_ptr< DrawSystem > m_pDrawSystem;
};
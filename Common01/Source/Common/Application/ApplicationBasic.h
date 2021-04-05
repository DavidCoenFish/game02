#pragma once

#include "Common/Application/IApplication.h"
class ApplicationHolder;

class ApplicationBasic : public IApplication
{
public:
   ApplicationBasic(const IApplicationParam& applicationParam);
   virtual ~ApplicationBasic();

};
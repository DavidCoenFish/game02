#pragma once

class ApplicationHolder;
class IApplication;
class IApplicationParam;

const int WindowHelper(
   const std::shared_ptr<ApplicationHolder>& pApplicationHolder,
   const std::function< IApplication*(const IApplicationParam&) >& pApplicationFactory,
   HINSTANCE hInstance,
   const std::string& applicationName,
   const bool bFullScreen,
   const int defaultWidth,
   const int defaultHeight,
   const int nCmdShow
   );


#pragma once

class ApplicationHolder;
class IApplication;
class IApplicationParam;
class CommandLine;

const int WindowHelper(
   const std::shared_ptr<ApplicationHolder>& pApplicationHolder,
   const std::function< IApplication*(const IApplicationParam&) >& pApplicationFactory,
   HINSTANCE hInstance,
   const std::string& applicationName,
   const bool bFullScreen,
   const int defaultWidth,
   const int defaultHeight,
   const std::shared_ptr< CommandLine >& pCommandLine,
   const int nCmdShow
   );


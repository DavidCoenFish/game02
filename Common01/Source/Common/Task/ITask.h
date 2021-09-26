#pragma once

#include "json/json.hpp"
class CommandLine;
class ITask;

typedef std::function< std::shared_ptr< ITask >(
   const HINSTANCE hInstance, 
   const int nCmdShow, 
   const std::shared_ptr< CommandLine >& pCommandLine, 
   const std::filesystem::path& path, 
   const nlohmann::json& json
   ) > TTaskFactory;

class ITask
{
public:
   virtual ~ITask();

   virtual const int Run() = 0;

};

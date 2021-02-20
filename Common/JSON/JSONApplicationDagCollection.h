#pragma once

#include "SDK/json/json.hpp"
class ApplicationDagCollection;
class CustomCommandList;
class DrawSystem;
class FileCache;

namespace JSONApplicationDagCollection
{
   std::unique_ptr<ApplicationDagCollection> Factory(
      DrawSystem* const pDrawSystem,
      CustomCommandList* const pCommandList,
      FileCache* const pFileCache,
      const nlohmann::json& json,
      const int width, 
      const int height
      );

};

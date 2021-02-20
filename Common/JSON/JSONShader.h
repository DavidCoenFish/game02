#pragma once

#include "SDK/json/json.hpp"
class DrawSystem;
class CustomCommandList;
class Shader;
class FileCache;

namespace JSONShader
{
   std::shared_ptr< Shader > Factory(
      DrawSystem* const pDrawSystem,
      CustomCommandList* const pCommandList,
      FileCache* const pFileCache,
      const nlohmann::json& json
      );
};

#pragma once

#include "SDK/json/json.hpp"
class DrawSystem;
class CustomCommandList;
class Geometry;

namespace JSONGeometry
{
   std::shared_ptr< Geometry > Factory(
      DrawSystem* const pDrawSystem,
      CustomCommandList* const pCommandList,
      const nlohmann::json& json
      );
};

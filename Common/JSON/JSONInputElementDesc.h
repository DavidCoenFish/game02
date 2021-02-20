#pragma once

#include "SDK/json/json.hpp"
class DrawSystem;
class CustomCommandList;
class Geometry;

namespace JSONInputElementDesc
{
   std::vector<D3D12_INPUT_ELEMENT_DESC> Factory(
      const nlohmann::json& json
      );
};

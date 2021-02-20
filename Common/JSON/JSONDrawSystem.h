#pragma once

#include "SDK/json/json.hpp"

class DrawSystem;

namespace JSONDrawSystem
{
   std::unique_ptr<DrawSystem> Factory(
      const std::function< void(void) >& callbackFatalExit,
      const nlohmann::json& json,
      HWND hwnd, 
      const int width, 
      const int height
      );

};

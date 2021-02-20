//
// pch.cpp
// Include the standard header and generate the precompiled header.
//

#include "CommonPCH.h"
#include "Common/JSON/JSONDrawSystem.h"
#include "Common/JSON/JSONRenderTarget.h"
#include "Common/JSON/JSONEnum.h"
#include "Common/Render/DrawSystem.h"

struct JSONDrawSystemData
{
   JSONDrawSystemData()
   {
      backBufferCount = 2;
      flags = 0;
      d3dMinFeatureLevel = D3D_FEATURE_LEVEL_11_0;
   }

   int backBufferCount;
   unsigned int flags;
   D3D_FEATURE_LEVEL d3dMinFeatureLevel;
   RenderTargetFormatData backBuffer;
   RenderTargetDepthData backDepth;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONDrawSystemData, 
   backBufferCount,
   flags,
   d3dMinFeatureLevel,
   backBuffer,
   backDepth
   );

std::unique_ptr<DrawSystem> JSONDrawSystem::Factory(
   const std::function< void(void) >& callbackFatalExit,
   const nlohmann::json& jsonParent,
   HWND hwnd, 
   const int width, 
   const int height
   )
{
   JSONDrawSystemData data;
   if (jsonParent.contains("DrawSystem"))
   {
      const nlohmann::json& json = jsonParent["DrawSystem"];
      data = json.get< JSONDrawSystemData >();
   }

   return std::make_unique< DrawSystem >(
      callbackFatalExit,
      hwnd, 
      width,
      height,
      data.backBufferCount,
      data.backBuffer,
      data.backDepth,
      data.flags,
      data.d3dMinFeatureLevel
      );
}

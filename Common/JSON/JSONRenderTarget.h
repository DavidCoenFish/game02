#pragma once

#include "SDK/json/json.hpp"
#include "Common/JSON/JSONVector.h"
#include "Common/Render/RenderTargetFormatData.h"
#include "Common/Render/RenderTargetDepthData.h"
class DrawSystem;
class CustomCommandList;
class Shader;
class FileCache;

//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
//   RenderTargetFormatData, 
//   format,
//   clearOnSet,
//   clearColor
//   );
void to_json(nlohmann::json& j, const RenderTargetFormatData& p);
void from_json(const nlohmann::json& j, RenderTargetFormatData& p);

//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
//   RenderTargetDepthData, 
//   format,
//   clearDepthOnSet,
//   clearDepth,
//   clearStencilOnSet,
//   clearStencil,
//   shaderResource
//   );
void to_json(nlohmann::json& j, const RenderTargetDepthData& p);
void from_json(const nlohmann::json& j, RenderTargetDepthData& p);


struct JSONRenderTarget
{
   std::vector< RenderTargetFormatData > targetBufferDataArray;
   RenderTargetDepthData targetDepthData;
   int width;
   int height;
   bool resizeWithScreen;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONRenderTarget, 
   targetBufferDataArray,
   targetDepthData,
   width,
   height,
   resizeWithScreen
   );

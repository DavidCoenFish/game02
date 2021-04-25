#pragma once

#include "json/json.hpp"
#include "Common/DrawSystem/Shader/ShaderPipelineStateData.h"
#include "Common/JSON/JSONDrawSystemEnum.h"

void to_json(nlohmann::json& j, const ShaderPipelineStateData& p);
void from_json(const nlohmann::json& j, ShaderPipelineStateData& p);

struct JSONShader
{
   ShaderPipelineStateData pipelineState;
   std::string vertexShader;
   std::string geometryShader;
   std::string pixelShader;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONShader,
   pipelineState,
   vertexShader,
   geometryShader,
   pixelShader
   );

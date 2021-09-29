#pragma once

#include "json/json.hpp"

struct ShaderConstantInfo;

//void to_json(nlohmann::json& j, const ShaderConstantInfo& p);
//void from_json(const nlohmann::json& j, ShaderConstantInfo& p);
//void from_json(const nlohmann::json& j, std::vector< std::shared_ptr< ShaderConstantInfo > >& p);

struct JSONShaderConstantInfo
{
   JSONShaderConstantInfo()
      : floatCount(0)
      , visiblity(D3D12_SHADER_VISIBILITY_ALL)
   {
      //nop
   }
   int floatCount;
   D3D12_SHADER_VISIBILITY visiblity;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONShaderConstantInfo,
   floatCount,
   visiblity
   );

const std::vector< std::shared_ptr< ShaderConstantInfo > > TransformShaderConstantInfo(const std::vector< JSONShaderConstantInfo >& input); 

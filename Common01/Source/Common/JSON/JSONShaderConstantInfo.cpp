#include "CommonPCH.h"
#include "Common/JSON/JSONShaderConstantInfo.h"
#include "Common/JSON/JSONDrawSystemEnum.h"
#include "Common/DrawSystem/Shader/ShaderConstantInfo.h"

//void to_json(nlohmann::json& j, const ShaderConstantInfo& p)
//{
//   j; p;
//}
//
//void from_json(const nlohmann::json& j, ShaderConstantInfo& p)
//{
//   JSONShaderConstantInfo info;
//   j.get_to(info);
//   p.m_data.resize(sizeof(float) * info.floatCount);
//   p.m_visiblity = info.visiblity;
//
//   return;
//}
//
//void from_json(const nlohmann::json& j, std::vector< std::shared_ptr< ShaderConstantInfo > >& p)
const std::vector< std::shared_ptr< ShaderConstantInfo > > TransformShaderConstantInfo(const std::vector< JSONShaderConstantInfo >& input)
{
   //p.clear();

   ////auto wrapper = json.get<JSONInputElementDescWrapper>();
   //std::vector< JSONShaderConstantInfo > jsonDataArray;
   //j.get_to(jsonDataArray);
   std::vector< std::shared_ptr< ShaderConstantInfo > > output;
   for (const auto& item : input)
   {
      std::vector< uint8_t > data;
      data.resize(sizeof(float) * item.floatCount);
      output.push_back(std::make_shared< ShaderConstantInfo >(data, item.visiblity));
   }
   return output;
}


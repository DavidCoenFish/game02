#include "CommonPCH.h"
#include "Common/JSON/JSONShaderConstantInfo.h"
#include "Common/JSON/JSONDrawSystemEnum.h"
#include "Common/DrawSystem/Shader/ShaderConstantInfo.h"

const std::vector< std::shared_ptr< ShaderConstantInfo > > TransformShaderConstantInfo(const std::vector< JSONShaderConstantInfo >& input)
{
   std::vector< std::shared_ptr< ShaderConstantInfo > > output;
   for (const auto& item : input)
   {
      std::vector< uint8_t > data;
      data.resize(sizeof(float) * item.floatCount);
      output.push_back(std::make_shared< ShaderConstantInfo >(data, item.visiblity));
   }
   return output;
}


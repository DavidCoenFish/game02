#include "CommonPCH.h"
#include "Common/JSON/JSONShader.h"
#include "Common/JSON/JSONEnum.h"
#include "Common/JSON/JSONInputElementDesc.h"
#include "Common/Render/CustomCommandList.h"
#include "Common/Render/DrawSystem.h"
#include "Common/Render/Shader.h"
#include "Common/Render/ShaderPipelineStateData.h"
#include "Common/Render/ShaderResourceInfo.h"
#include "Common/Render/ShaderConstantBuffer.h"
#include "Common/Application/FileCache.h"
#include "Common/Utils/Utf8.h"

NLOHMANN_JSON_SERIALIZE_ENUM( D3D12_PRIMITIVE_TOPOLOGY_TYPE, {
   {D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED, nullptr},
   {TOKEN_PAIR(D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED)},
   {TOKEN_PAIR(D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED)},
   {TOKEN_PAIR(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT)},
   {TOKEN_PAIR(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)},
   {TOKEN_PAIR(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)},
   {TOKEN_PAIR(D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH)}
   });

NLOHMANN_JSON_SERIALIZE_ENUM( D3D12_BLEND, {
   {D3D12_BLEND_ZERO, nullptr},
   {TOKEN_PAIR(D3D12_BLEND_ZERO)},
   {TOKEN_PAIR(D3D12_BLEND_ONE)},
   {TOKEN_PAIR(D3D12_BLEND_SRC_COLOR)},
   {TOKEN_PAIR(D3D12_BLEND_INV_SRC_COLOR)},
   {TOKEN_PAIR(D3D12_BLEND_SRC_ALPHA)},
   {TOKEN_PAIR(D3D12_BLEND_INV_SRC_ALPHA)},
   {TOKEN_PAIR(D3D12_BLEND_DEST_ALPHA)},
   {TOKEN_PAIR(D3D12_BLEND_INV_DEST_ALPHA)},
   {TOKEN_PAIR(D3D12_BLEND_DEST_COLOR)},
   {TOKEN_PAIR(D3D12_BLEND_INV_DEST_COLOR)},
   {TOKEN_PAIR(D3D12_BLEND_SRC_ALPHA_SAT)},
   {TOKEN_PAIR(D3D12_BLEND_BLEND_FACTOR)},
   {TOKEN_PAIR(D3D12_BLEND_INV_BLEND_FACTOR)},
   {TOKEN_PAIR(D3D12_BLEND_SRC1_COLOR)},
   {TOKEN_PAIR(D3D12_BLEND_INV_SRC1_COLOR)},
   {TOKEN_PAIR(D3D12_BLEND_SRC1_ALPHA)},
   {TOKEN_PAIR(D3D12_BLEND_INV_SRC1_ALPHA)}
   });

NLOHMANN_JSON_SERIALIZE_ENUM( D3D12_DEPTH_WRITE_MASK, {
   {TOKEN_PAIR(D3D12_DEPTH_WRITE_MASK_ZERO)},
   {TOKEN_PAIR(D3D12_DEPTH_WRITE_MASK_ALL)},
   });

NLOHMANN_JSON_SERIALIZE_ENUM( D3D12_COMPARISON_FUNC, {
   {TOKEN_PAIR(D3D12_COMPARISON_FUNC_NEVER)},
   {TOKEN_PAIR(D3D12_COMPARISON_FUNC_LESS)},
   {TOKEN_PAIR(D3D12_COMPARISON_FUNC_EQUAL)},
   {TOKEN_PAIR(D3D12_COMPARISON_FUNC_LESS_EQUAL)},
   {TOKEN_PAIR(D3D12_COMPARISON_FUNC_GREATER)},
   {TOKEN_PAIR(D3D12_COMPARISON_FUNC_NOT_EQUAL)},
   {TOKEN_PAIR(D3D12_COMPARISON_FUNC_GREATER_EQUAL)},
   {TOKEN_PAIR(D3D12_COMPARISON_FUNC_ALWAYS)}
   });

NLOHMANN_JSON_SERIALIZE_ENUM( D3D12_FILTER, {
   {TOKEN_PAIR(D3D12_FILTER_MIN_MAG_MIP_POINT)},
   {TOKEN_PAIR(D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR)},
   {TOKEN_PAIR(D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT)},
   {TOKEN_PAIR(D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR)},
   {TOKEN_PAIR(D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT)},
   {TOKEN_PAIR(D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR)},
   {TOKEN_PAIR(D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT)},
   {TOKEN_PAIR(D3D12_FILTER_MIN_MAG_MIP_LINEAR)},
   {TOKEN_PAIR(D3D12_FILTER_ANISOTROPIC)},
   {TOKEN_PAIR(D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT)},
   {TOKEN_PAIR(D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR)},
   {TOKEN_PAIR(D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT)},
   {TOKEN_PAIR(D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR)},
   {TOKEN_PAIR(D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT)},
   {TOKEN_PAIR(D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR)},
   {TOKEN_PAIR(D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT)},
   {TOKEN_PAIR(D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR)},
   {TOKEN_PAIR(D3D12_FILTER_COMPARISON_ANISOTROPIC)},
   {TOKEN_PAIR(D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT)},
   {TOKEN_PAIR(D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR)},
   {TOKEN_PAIR(D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT)},
   {TOKEN_PAIR(D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR)},
   {TOKEN_PAIR(D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT)},
   {TOKEN_PAIR(D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR)},
   {TOKEN_PAIR(D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT)},
   {TOKEN_PAIR(D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR)},
   {TOKEN_PAIR(D3D12_FILTER_MINIMUM_ANISOTROPIC)},
   {TOKEN_PAIR(D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT)},
   {TOKEN_PAIR(D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR)},
   {TOKEN_PAIR(D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT)},
   {TOKEN_PAIR(D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR)},
   {TOKEN_PAIR(D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT)},
   {TOKEN_PAIR(D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR)},
   {TOKEN_PAIR(D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT)},
   {TOKEN_PAIR(D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR)},
   {TOKEN_PAIR(D3D12_FILTER_MAXIMUM_ANISOTROPIC)}
   });

NLOHMANN_JSON_SERIALIZE_ENUM( D3D12_TEXTURE_ADDRESS_MODE, {
   {TOKEN_PAIR(D3D12_TEXTURE_ADDRESS_MODE_WRAP)},
   {TOKEN_PAIR(D3D12_TEXTURE_ADDRESS_MODE_MIRROR)},
   {TOKEN_PAIR(D3D12_TEXTURE_ADDRESS_MODE_CLAMP)},
   {TOKEN_PAIR(D3D12_TEXTURE_ADDRESS_MODE_BORDER)},
   {TOKEN_PAIR(D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE)}
   });

NLOHMANN_JSON_SERIALIZE_ENUM( D3D12_STATIC_BORDER_COLOR, {
   {TOKEN_PAIR(D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK)},
   {TOKEN_PAIR(D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK)},
   {TOKEN_PAIR(D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE)}
   });

NLOHMANN_JSON_SERIALIZE_ENUM( D3D12_SHADER_VISIBILITY, {
   {TOKEN_PAIR(D3D12_SHADER_VISIBILITY_ALL)},
   {TOKEN_PAIR(D3D12_SHADER_VISIBILITY_VERTEX)},
   {TOKEN_PAIR(D3D12_SHADER_VISIBILITY_HULL)},
   {TOKEN_PAIR(D3D12_SHADER_VISIBILITY_DOMAIN)},
   {TOKEN_PAIR(D3D12_SHADER_VISIBILITY_GEOMETRY)},
   {TOKEN_PAIR(D3D12_SHADER_VISIBILITY_PIXEL)},
   {TOKEN_PAIR(D3D12_SHADER_VISIBILITY_AMPLIFICATION)},
   {TOKEN_PAIR(D3D12_SHADER_VISIBILITY_MESH)}
   });

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   D3D12_RENDER_TARGET_BLEND_DESC,
    BlendEnable,
    LogicOpEnable,
    SrcBlend,
    DestBlend,
    BlendOp,
    SrcBlendAlpha,
    DestBlendAlpha,
    BlendOpAlpha,
    LogicOp,
    RenderTargetWriteMask
    );
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   D3D12_BLEND_DESC,
   AlphaToCoverageEnable,
   IndependentBlendEnable,
   RenderTarget
   );
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   D3D12_RASTERIZER_DESC,
   FillMode,
   CullMode,
   FrontCounterClockwise,
   DepthBias,
   DepthBiasClamp,
   SlopeScaledDepthBias,
   DepthClipEnable,
   MultisampleEnable,
   AntialiasedLineEnable,
   ForcedSampleCount,
   ConservativeRaster
   );

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   D3D12_DEPTH_STENCILOP_DESC,
   StencilFailOp,
   StencilDepthFailOp,
   StencilPassOp,
   StencilFunc
   );

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   D3D12_DEPTH_STENCIL_DESC,
   DepthEnable,
   DepthWriteMask,
   DepthFunc,
   StencilEnable,
   StencilReadMask,
   StencilWriteMask,
   FrontFace,
   BackFace
   );

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   D3D12_STATIC_SAMPLER_DESC,
   Filter,
   AddressU,
   AddressV,
   AddressW,
   MipLODBias,
   MaxAnisotropy,
   ComparisonFunc,
   BorderColor,
   MinLOD,
   MaxLOD,
   ShaderRegister,
   RegisterSpace,
   ShaderVisibility
   );

struct JSONShaderPipelineStateData
{
   explicit JSONShaderPipelineStateData()
      : topologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED)
      , depthFormat(DXGI_FORMAT_UNKNOWN)
      , renderTargetFormat()
      , blendState(CD3DX12_BLEND_DESC(CD3DX12_DEFAULT()))
      , rasterizerState(CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT()))
      , depthStencilState(CD3DX12_DEPTH_STENCIL_DESC())
   {
      return;
   }
   D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType;
   DXGI_FORMAT depthFormat;
   std::vector<DXGI_FORMAT> renderTargetFormat;
   D3D12_BLEND_DESC blendState;
   D3D12_RASTERIZER_DESC rasterizerState;
   D3D12_DEPTH_STENCIL_DESC depthStencilState;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONShaderPipelineStateData,
   topologyType,
   depthFormat,
   renderTargetFormat,
   blendState,
   rasterizerState,
   depthStencilState
);

struct JSONShaderResourceInfo
{
   JSONShaderResourceInfo()
      : bUseSampler(false)
   {
      memset(&samplerDesc, 0, sizeof(D3D12_STATIC_SAMPLER_DESC));
      return;
   }
   D3D12_STATIC_SAMPLER_DESC samplerDesc;
   bool bUseSampler;
};

void to_json(nlohmann::json& j, const JSONShaderResourceInfo& p)
{
   j = nlohmann::json::object();
   j["useSampler"] = p.bUseSampler;
   j["samplerDesc"] = p.samplerDesc;
}
void from_json(const nlohmann::json& j, JSONShaderResourceInfo& p)
{
   if(j.contains("samplerDesc"))
   {
      p.samplerDesc = j.get< D3D12_STATIC_SAMPLER_DESC >();
      if(j.contains("useSampler"))
      { 
         j.at("useSampler").get_to(p.bUseSampler); 
      }
      else
      {
         p.bUseSampler = true;
      }
   }
}

struct JSONShaderConstantInfo
{
   JSONShaderConstantInfo()
      : visiblity(D3D12_SHADER_VISIBILITY_ALL)
   {
      return;
   }
   D3D12_SHADER_VISIBILITY visiblity;
   std::vector< uint8_t > data;
};
//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
//   JSONShaderConstantInfo,
//   visiblity,
//   dataFloat,
//   data
//   );
//#define NLOHMANN_JSON_TO(v1) nlohmann_json_j[#v1] = nlohmann_json_t.v1;
//#define NLOHMANN_JSON_FROM(v1) if(nlohmann_json_j.contains(#v1)){ nlohmann_json_j.at(#v1).get_to(nlohmann_json_t.v1); }
void to_json(nlohmann::json& j, const JSONShaderConstantInfo& p)
{
   j["visiblity"] = p.visiblity;
   j["data"] = p.data;
}

void from_json(const nlohmann::json& j, JSONShaderConstantInfo& p)
{
   if(j.contains("visiblity"))
   { 
      j.at("visiblity").get_to(p.visiblity); 
   }
   if(j.contains("data"))
   { 
      j.at("data").get_to(p.data); 
   }
   if(j.contains("dataFloat"))
   { 
      std::vector<float> data;
      j.at("data").get_to(data); 
      for (const float& item : data)
      {
         const uint8_t* const pTrace = (const uint8_t* const)&item;
         p.data.push_back(pTrace[0]);
         p.data.push_back(pTrace[1]);
         p.data.push_back(pTrace[2]);
         p.data.push_back(pTrace[3]);
      }
   }
}


struct JSONShaderData
{
   JSONShaderPipelineStateData pipelineState;
   std::string vertexShader;
   std::string pixelShader;
   std::string geometryShader;
   std::vector< JSONShaderResourceInfo> shaderResourceInfo;
   std::vector< JSONShaderConstantInfo> shaderConstantInfo;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONShaderData, 
   pipelineState,
   vertexShader,
   pixelShader,
   geometryShader,
   shaderResourceInfo,
   shaderConstantInfo
   );

static std::shared_ptr< std::vector<uint8_t> > DealFile(FileCache* const pFileCache, const std::string& filePath)
{
   if (true == filePath.empty())
   {
      return nullptr;
   }
   auto wPath = Utf8::Utf8ToUtf16( filePath );
   auto pResult = pFileCache->RequestFile(wPath);
   return pResult;
}

std::shared_ptr< Shader > JSONShader::Factory(
   DrawSystem* const pDrawSystem,
   CustomCommandList* const pCommandList,
   FileCache* const pFileCache,
   const nlohmann::json& json
   )
{
   //std::vector< D3D12_INPUT_ELEMENT_DESC > inputElementDescArray = JSONInputElementDesc::Factory(json);

   auto shaderData = json.get< JSONShaderData >();
   auto pVertexShaderData = DealFile(pFileCache, shaderData.vertexShader);
   auto pGeometryShaderData = DealFile(pFileCache, shaderData.geometryShader);
   auto pPixelShaderData = DealFile(pFileCache, shaderData.pixelShader);
   auto& pipelineState = shaderData.pipelineState;
   std::vector< D3D12_INPUT_ELEMENT_DESC > inputElementDesc = JSONInputElementDesc::Factory(json);

   ShaderPipelineStateData data(
      inputElementDesc,
      pipelineState.topologyType,
      pipelineState.depthFormat,
      pipelineState.renderTargetFormat,
      pipelineState.blendState,
      pipelineState.rasterizerState,
      pipelineState.depthStencilState
      );

   std::vector< std::shared_ptr< ShaderResourceInfo > > arrayShaderResourceInfo;
   for (const auto& jsonShaderResourceInfo : shaderData.shaderResourceInfo)
   {
      arrayShaderResourceInfo.push_back(
         std::make_shared<ShaderResourceInfo>(
            nullptr,
            jsonShaderResourceInfo.samplerDesc,
            jsonShaderResourceInfo.bUseSampler
            )
         );
   }
   std::vector< std::shared_ptr< ShaderConstantInfo > > arrayShaderConstantsInfo;
   for (const auto& jsonShaderConstantInfo : shaderData.shaderConstantInfo)
   {
      arrayShaderConstantsInfo.push_back(
         std::make_shared<ShaderConstantInfo>(
            jsonShaderConstantInfo.data,
            jsonShaderConstantInfo.visiblity
            )
         );
   }

   auto pResult = pDrawSystem->ShaderFactory(
      pCommandList,
      data,
      pVertexShaderData, 
      pGeometryShaderData,
      pPixelShaderData,
      arrayShaderResourceInfo,
      arrayShaderConstantsInfo
      );

  return pResult;
}

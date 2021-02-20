#include "CommonPCH.h"
#include "Common/JSON/JSONShader.h"
#include "Common/JSON/JSONEnum.h"
#include "Common/JSON/JSONInputElementDesc.h"
#include "Common/Render/CustomCommandList.h"
#include "Common/Render/DrawSystem.h"
#include "Common/Render/Shader.h"
#include "Common/Render/ShaderPipelineStateData.h"
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

struct JSONShaderData
{
   explicit JSONShaderData()
      : resourceCount(0)
      , constantsCount(0)
   {
      return;
   }
   std::string vertexShader;
   std::string pixelShader;
   std::string geometryShader;
   int resourceCount;
   int constantsCount;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONShaderData, 
   vertexShader,
   pixelShader,
   geometryShader,
   resourceCount,
   constantsCount
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
   std::vector< D3D12_INPUT_ELEMENT_DESC > inputElementDescArray = JSONInputElementDesc::Factory(json);

   auto shaderData = json.get< JSONShaderData >();
   auto pVertexShaderData = DealFile(pFileCache, shaderData.vertexShader);
   auto pGeometryShaderData = DealFile(pFileCache, shaderData.geometryShader);
   auto pPixelShaderData = DealFile(pFileCache, shaderData.pixelShader);
   auto pipelineState = json.get< JSONShaderPipelineStateData >();

   ShaderPipelineStateData data(
      inputElementDescArray,
      pipelineState.topologyType,
      pipelineState.depthFormat,
      pipelineState.renderTargetFormat,
      pipelineState.blendState,
      pipelineState.rasterizerState,
      pipelineState.depthStencilState
      );

   std::vector< std::shared_ptr< ShaderResourceInfo > > arrayShaderResourceInfo;
   for (int index = 0; index < shaderData.resourceCount; ++index)
   {

   }
   std::vector< std::shared_ptr< ShaderConstantInfo > > arrayShaderConstantsInfo;

   auto pResult = pDrawSystem->ShaderFactory(
      pCommandList,
      data,
      pVertexShaderData, 
      pGeometryShaderData,
      pPixelShaderData
      );

  return pResult;
}

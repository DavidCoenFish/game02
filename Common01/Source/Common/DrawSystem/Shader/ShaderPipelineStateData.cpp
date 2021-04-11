#include "CommonPCH.h"

#include "Common/DrawSystem/Shader/ShaderPipelineStateData.h"

ShaderPipelineStateData::ShaderPipelineStateData(
   const std::vector< D3D12_INPUT_ELEMENT_DESC >& inputElementDescArray,
   const D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType,
   const DXGI_FORMAT depthStencilViewFormat,
   const std::vector<DXGI_FORMAT>& renderTargetFormat,
   const D3D12_BLEND_DESC& blendState,
   const D3D12_RASTERIZER_DESC& rasterizerState,
   const D3D12_DEPTH_STENCIL_DESC& depthStencilState
   )
   : m_inputElementDescArray(inputElementDescArray)
   , m_primitiveTopologyType(primitiveTopologyType)
   , m_depthStencilViewFormat(depthStencilViewFormat)
   , m_renderTargetFormat(renderTargetFormat)
   , m_blendState(blendState)
   , m_rasterizerState(rasterizerState)
   , m_depthStencilState(depthStencilState)
{
   return;
}

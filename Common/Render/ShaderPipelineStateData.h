#pragma once

struct ShaderPipelineStateData
{
   explicit ShaderPipelineStateData(
      const std::vector< D3D12_INPUT_ELEMENT_DESC >& inputElementDescArray = std::vector< D3D12_INPUT_ELEMENT_DESC >(),
      const D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,
      const DXGI_FORMAT depthStencilViewFormat = DXGI_FORMAT_UNKNOWN,
      const std::vector<DXGI_FORMAT>& renderTargetFormat = std::vector<DXGI_FORMAT>(),
      const D3D12_BLEND_DESC& blendState = CD3DX12_BLEND_DESC(), //D3D12_DEFAULT),
      const D3D12_RASTERIZER_DESC& rasterizerState = CD3DX12_RASTERIZER_DESC(), //D3D12_DEFAULT),
      const D3D12_DEPTH_STENCIL_DESC& depthStencilState = CD3DX12_DEPTH_STENCIL_DESC() //D3D12_DEFAULT)
      );

   const std::vector< D3D12_INPUT_ELEMENT_DESC > m_inputElementDescArray;
   const D3D12_PRIMITIVE_TOPOLOGY_TYPE m_primitiveTopologyType;
   const DXGI_FORMAT m_depthStencilViewFormat;
   const std::vector<DXGI_FORMAT> m_renderTargetFormat;
   const D3D12_BLEND_DESC m_blendState;
   const D3D12_RASTERIZER_DESC m_rasterizerState;
   const D3D12_DEPTH_STENCIL_DESC m_depthStencilState;
};


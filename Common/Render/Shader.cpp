#include "CommonPCH.h"

#include "Common/Render/Shader.h"

#include "Common/Render/DrawSystem.h"
#include "Common/Render/ShaderConstantBuffer.h"
#include "Common/Render/ShaderConstantInfo.h"
#include "Common/Render/ShaderResourceInfo.h"

std::shared_ptr< ShaderConstantBuffer > MakeConstantBuffer(
   DrawSystem* const pDrawSystem,
   const std::shared_ptr< ShaderConstantInfo >& pShaderConstantInfo
   )
{
   if (nullptr == pShaderConstantInfo)
   {
      return nullptr;
   }
   const size_t constantBufferSize = pShaderConstantInfo->GetBufferSize();
   const void* const pData = pShaderConstantInfo->GetBufferData();
   const D3D12_SHADER_VISIBILITY visiblity = pShaderConstantInfo->GetVisiblity();
   const int frameCount = pDrawSystem->GetBackBufferCount();
   auto pConstantBuffer = std::make_shared< ShaderConstantBuffer >( 
      frameCount, 
      constantBufferSize, 
      pDrawSystem->MakeHeapWrapperCbvSrvUav(),
      pData,
      visiblity
      );
   return pConstantBuffer;
}

static Microsoft::WRL::ComPtr<ID3D12RootSignature> MakeRootSignatureLocal(
   ID3D12Device* const pDevice,
   const D3D12_ROOT_SIGNATURE_DESC& rootSignatureDesc
   )
{
   Microsoft::WRL::ComPtr<ID3D12RootSignature> pRootSignature;
   {
      Microsoft::WRL::ComPtr<ID3DBlob> signature;
      Microsoft::WRL::ComPtr<ID3DBlob> error;
      DX::ThrowIfFailed(
         D3D12SerializeRootSignature(
            &rootSignatureDesc, 
            D3D_ROOT_SIGNATURE_VERSION_1, 
            &signature, 
            &error
            )
         );
      DX::ThrowIfFailed(
         pDevice->CreateRootSignature(
            0, 
            signature->GetBufferPointer(), 
            signature->GetBufferSize(),
            IID_PPV_ARGS(pRootSignature.ReleaseAndGetAddressOf())
            )
         );
   }
   static int s_trace = 0;
   const std::wstring name = (std::wstring(L"RootSignature:") + std::to_wstring(s_trace++)).c_str();
   pRootSignature->SetName(name.c_str());
   OutputDebugStringW(name.c_str());
   OutputDebugStringW(L"\n");

   return pRootSignature;

}

static void RemoveDenyFlag(D3D12_ROOT_SIGNATURE_FLAGS& flag, const D3D12_SHADER_VISIBILITY visiblity)
{
   switch (visiblity)
   {
   default:
      break;
   case D3D12_SHADER_VISIBILITY_ALL:
      flag &= ~(
         D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
         D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
         D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
         D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
         D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS |
         D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
         D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS
         );
      break;
   case D3D12_SHADER_VISIBILITY_VERTEX:
      flag &= ~(
         D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS
         );
      break;
   case D3D12_SHADER_VISIBILITY_HULL:
      flag &= ~(
         D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
         );
      break;
   case D3D12_SHADER_VISIBILITY_DOMAIN:
      flag &= ~(
         D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
         );
      break;
   case D3D12_SHADER_VISIBILITY_GEOMETRY:
      flag &= ~(
         D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
         );
      break;
   case D3D12_SHADER_VISIBILITY_PIXEL:
      flag &= ~(
         D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS
         );
      break;
   case D3D12_SHADER_VISIBILITY_AMPLIFICATION:
      flag &= ~(
         D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS
         );
      break;
   case D3D12_SHADER_VISIBILITY_MESH:
      flag &= ~(
         D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS
         );
      break;
   }
   return;
}


Microsoft::WRL::ComPtr<ID3D12RootSignature> MakeRootSignature(
   ID3D12Device* const pDevice,
   const std::vector< std::shared_ptr< ShaderResourceInfo > >& shaderTextureInfoArray,
   const std::vector< std::shared_ptr< ShaderConstantBuffer > >& constantBufferArray
   )
{
   Microsoft::WRL::ComPtr<ID3D12RootSignature> pRootSignature;

   {
      D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;
      std::vector<D3D12_ROOT_PARAMETER> rootParamterArray;

      std::vector< std::shared_ptr< D3D12_DESCRIPTOR_RANGE > > arrayDescriptorRange;
      std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplerDescArray;
      int trace = 0;
      int samplerTrace = 0;
      for (const auto& item : shaderTextureInfoArray)
      {
         auto pDescriptorRange = std::make_shared< D3D12_DESCRIPTOR_RANGE >(
            D3D12_DESCRIPTOR_RANGE{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)trace, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND }
            );
         trace += 1;

         arrayDescriptorRange.push_back(pDescriptorRange);
         auto visiblity = item->GetVisiblity();
         D3D12_ROOT_PARAMETER rootParameter = {
            D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            D3D12_ROOT_DESCRIPTOR_TABLE{ 1, pDescriptorRange.get() },
            visiblity
         };
         rootParamterArray.push_back(rootParameter);
         RemoveDenyFlag(flags, visiblity);

         if (item->GetUseSampler())
         {
            staticSamplerDescArray.push_back(item->GetStaticSamplerDesc());
            staticSamplerDescArray.back().ShaderRegister = samplerTrace;
            samplerTrace += 1;
         }
      }
      trace = 0;
      for (const auto& item : constantBufferArray)
      {
         auto pDescriptorRange = std::make_shared< D3D12_DESCRIPTOR_RANGE >(
            D3D12_DESCRIPTOR_RANGE{ D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, (UINT)trace, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND }
            );
         arrayDescriptorRange.push_back(pDescriptorRange);
         auto visiblity = item->GetVisiblity();
         D3D12_ROOT_PARAMETER rootParameter = {
            D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            D3D12_ROOT_DESCRIPTOR_TABLE{ 1, pDescriptorRange.get() },
            visiblity
         };
         rootParamterArray.push_back(rootParameter);
         RemoveDenyFlag(flags, visiblity);

         trace += 1;
      }
      CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
      rootSignatureDesc.Init(
         (UINT)rootParamterArray.size(),
         rootParamterArray.size() ? &rootParamterArray[0] : nullptr,
         (UINT)staticSamplerDescArray.size(),
         staticSamplerDescArray.size() ? &staticSamplerDescArray[0] : nullptr,
         flags
         );
      pRootSignature = MakeRootSignatureLocal(pDevice, rootSignatureDesc);
   }
   return pRootSignature;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> MakePipelineState(
   ID3D12Device* const pDevice,
   const Microsoft::WRL::ComPtr<ID3D12RootSignature>& pRootSignature,
   const std::shared_ptr< std::vector<uint8_t>>& pVertexShaderData,
   const std::shared_ptr< std::vector<uint8_t>>& pGeometryShaderData,
   const std::shared_ptr< std::vector<uint8_t>>& pPixelShaderData,
   const ShaderPipelineStateData& pipelineStateData
   )
{
   Microsoft::WRL::ComPtr<ID3D12PipelineState> pPipelineState;
   // Describe and create the graphics pipeline state object (PSO).
   D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
   psoDesc.pRootSignature = pRootSignature.Get();
   if (nullptr != pVertexShaderData)
   {
      psoDesc.VS = { pVertexShaderData->data(), pVertexShaderData->size() };
   }
   if (nullptr != pPixelShaderData)
   {
      psoDesc.PS = { pPixelShaderData->data(), pPixelShaderData->size() };
   }
   //D3D12_SHADER_BYTECODE DS;
   //D3D12_SHADER_BYTECODE HS;
   if (nullptr != pGeometryShaderData)
   {
      psoDesc.GS = { pGeometryShaderData->data(), pGeometryShaderData->size() };
   }
   //D3D12_STREAM_OUTPUT_DESC StreamOutput;
   psoDesc.BlendState = pipelineStateData.m_blendState; //CD3DX12_BLEND_DESC(D3D12_DEFAULT);
   psoDesc.SampleMask = UINT_MAX;
   psoDesc.RasterizerState = pipelineStateData.m_rasterizerState; //CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
   psoDesc.DepthStencilState = pipelineStateData.m_depthStencilState;
   //psoDesc.DepthStencilState.DepthEnable = FALSE;
   //psoDesc.DepthStencilState.StencilEnable = FALSE;
   psoDesc.InputLayout = { &pipelineStateData.m_inputElementDescArray[0], (UINT)pipelineStateData.m_inputElementDescArray.size() };
   //D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;
   psoDesc.PrimitiveTopologyType = pipelineStateData.m_primitiveTopologyType;
   psoDesc.NumRenderTargets = (UINT)pipelineStateData.m_renderTargetFormat.size(); // (UINT)numRenderTargets; //1
   for (int index = 0; index < (int)psoDesc.NumRenderTargets; ++index)
   {
      psoDesc.RTVFormats[index] = pipelineStateData.m_renderTargetFormat[index]; //DXGI_FORMAT_B8G8R8A8_UNORM;
   }
   psoDesc.DSVFormat = pipelineStateData.m_depthStencilViewFormat; //DXGI_FORMAT_UNKNOWN; // m_deviceResources->GetDepthBufferFormat();
   psoDesc.SampleDesc.Count = 1;
   //UINT NodeMask;
   //D3D12_CACHED_PIPELINE_STATE CachedPSO;
   //D3D12_PIPELINE_STATE_FLAGS Flags;

   DX::ThrowIfFailed(
      pDevice->CreateGraphicsPipelineState(
         &psoDesc,
         IID_PPV_ARGS(pPipelineState.ReleaseAndGetAddressOf())
         )
      );

   pPipelineState->SetName(L"PipelineState");

   return pPipelineState;
}





Shader::Shader(
   DrawSystem* const pDrawSystem,
   const ShaderPipelineStateData& pipelineStateData,
   const std::shared_ptr< std::vector<uint8_t> >& pVertexShaderData,
   const std::shared_ptr< std::vector<uint8_t> >& pGeometryShaderData,
   const std::shared_ptr< std::vector<uint8_t> >& pPixelShaderData,
   const std::vector< std::shared_ptr< ShaderResourceInfo > >& arrayShaderResourceInfo,
   const std::vector< std::shared_ptr< ShaderConstantInfo > >& arrayShaderConstantsInfo
   )
   : iResource(pDrawSystem)
   , m_pipelineStateData(pipelineStateData)
   , m_pVertexShaderData(pVertexShaderData)
   , m_pGeometryShaderData(pGeometryShaderData)
   , m_pPixelShaderData(pPixelShaderData)
   , m_arrayShaderResourceInfo(arrayShaderResourceInfo)
   , m_arrayShaderConstantsInfo(arrayShaderConstantsInfo)
{
   for (auto& iter : m_arrayShaderConstantsInfo)
   {
      auto pResult = MakeConstantBuffer(pDrawSystem, iter);
      if (pResult)
      {
         m_arrayShaderConstantBuffer.push_back(pResult);
      }
   }

   return;
}

void Shader::SetDebugName( const std::string& name )
{
   m_debugName = name;
}

void Shader::SetActivate(
   ID3D12GraphicsCommandList* const pCommandList,
   const int frameIndex
   )
{
   pCommandList->SetGraphicsRootSignature(m_rootSignature.Get());
   pCommandList->SetPipelineState(m_pipelineState.Get());

   UINT trace = 0;
   for ( auto& iter : m_arrayShaderResourceInfo)
   {
      iter->Activate( pCommandList, trace);
      trace += 1;
   }

   for( auto& iter : m_arrayShaderConstantBuffer )
   {
      iter->Activate( pCommandList, trace, frameIndex);
      trace += 1;
   }
}

void Shader::OnDeviceLost()
{
   m_rootSignature.Reset();
   m_pipelineState.Reset();

   for( auto& pConstantBuffer : m_arrayShaderConstantBuffer )
   {
      pConstantBuffer->DeviceLost();
   }
}

void Shader::OnDeviceRestored(
   CustomCommandList* const,
   ID3D12Device* const pDevice
   )
{
   for( auto& pConstantBuffer : m_arrayShaderConstantBuffer )
   {
      pConstantBuffer->DeviceRestored(pDevice);
   }
   m_rootSignature = MakeRootSignature(
      pDevice,
      m_arrayShaderResourceInfo,
      m_arrayShaderConstantBuffer
      );
   m_pipelineState = MakePipelineState(
      pDevice,
      m_rootSignature,
      m_pVertexShaderData,
      m_pGeometryShaderData,
      m_pPixelShaderData,
      m_pipelineStateData
      );

   return;

}


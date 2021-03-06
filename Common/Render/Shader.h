#pragma once

#include "Common/Render/iResource.h"
#include "Common/Render/ShaderPipelineStateData.h"
#include "Common/Render/ShaderConstantInfo.h"

class ShaderConstantInfo;
class ShaderResourceInfo;
class ShaderConstantBuffer;

class Shader : public iResource
{
public:
   Shader(
      DrawSystem* const pDrawSystem,
      const ShaderPipelineStateData& pipelineStateData,
      const std::shared_ptr< std::vector<uint8_t> >& pVertexShaderData,
      const std::shared_ptr< std::vector<uint8_t> >& pGeometryShaderData,
      const std::shared_ptr< std::vector<uint8_t> >& pPixelShaderData,
      const std::vector< std::shared_ptr< ShaderResourceInfo > >& arrayShaderResourceInfo = std::vector< std::shared_ptr< ShaderResourceInfo > >(),
      const std::vector< std::shared_ptr< ShaderConstantInfo > >& arrayShaderConstantsInfo = std::vector< std::shared_ptr< ShaderConstantInfo > >()
      );

   void SetDebugName( const std::string& name );

   void SetActivate(
      ID3D12GraphicsCommandList* const pCommandList,
      const int frameIndex
      );

   template <typename CONSTANTS>
   CONSTANTS& GetConstant( const int index)
   {
      if ((0 <= index) && (index < m_arrayShaderConstantsInfo.size()))
      {
         assert(sizeof(CONSTANTS) == m_arrayShaderConstantsInfo[index]->GetBufferSize());
         const void* const pData = m_arrayShaderConstantsInfo[index]->GetBufferData();
         return (*((CONSTANTS*)pData));
      }
      CONSTANTS result;
      return result;
   }

private:
   virtual void OnDeviceLost() override;

   virtual void OnDeviceRestored(
      CustomCommandList* const,
      ID3D12Device* const pDevice
      ) override;

private:
   ShaderPipelineStateData m_pipelineStateData;
   std::shared_ptr< std::vector<uint8_t> > m_pVertexShaderData;
   std::shared_ptr< std::vector<uint8_t> > m_pGeometryShaderData;
   std::shared_ptr< std::vector<uint8_t> > m_pPixelShaderData;
   std::vector< std::shared_ptr< ShaderResourceInfo > > m_arrayShaderResourceInfo;
   std::vector< std::shared_ptr< ShaderConstantInfo > > m_arrayShaderConstantsInfo;

   std::vector< std::shared_ptr< ShaderConstantBuffer > > m_arrayShaderConstantBuffer;
   Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
   Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

   std::string m_debugName;
};

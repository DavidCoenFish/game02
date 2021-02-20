#pragma once

#include "Common/Render/iResource.h"
#include "Common/Render/ShaderTexture2D.h"
class HeapWrapperItem;

/*
shader buffer is a tpe of shader texture
*/
class ShaderBuffer : public iResource
{
public:
   ShaderBuffer(
      DrawSystem* const pDrawSystem,
      const std::shared_ptr< HeapWrapperItem >& shaderResource,
      const D3D12_RESOURCE_DESC& desc, 
      const D3D12_SHADER_RESOURCE_VIEW_DESC& shaderResourceViewDesc,
      //const int floatStride,
      const std::vector<float>& data
      );

   std::shared_ptr< HeapWrapperItem > GetHeapWrapperItem() const;


private:
   virtual void OnDeviceLost() override;

   virtual void OnDeviceRestored(
      CustomCommandList* const pCommandList,
      ID3D12Device* const pDevice
      ) override;


private:
   Microsoft::WRL::ComPtr<ID3D12Resource> m_pResource;
   std::shared_ptr< HeapWrapperItem > m_shaderResource;
   D3D12_RESOURCE_DESC m_desc;
   D3D12_SHADER_RESOURCE_VIEW_DESC m_shaderResourceViewDesc;
   //int m_floatStride;
   std::vector< float > m_data;

};

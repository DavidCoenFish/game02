#include "CommonPCH.h"

#include "Common/Render/ShaderBuffer.h"

ShaderBuffer::ShaderBuffer(
   DrawSystem* const pDrawSystem,
   const std::shared_ptr< HeapWrapperItem >& shaderResource,
   const D3D12_RESOURCE_DESC& desc, 
   const D3D12_SHADER_RESOURCE_VIEW_DESC& shaderResourceViewDesc,
   const std::vector<float>& data
   )
   : iResource( pDrawSystem )
   , m_pResource()
   , m_shaderResource(shaderResource)
   , m_desc(desc)
   , m_shaderResourceViewDesc(shaderResourceViewDesc)
   , m_data(data)
{
   return;
}

std::shared_ptr< HeapWrapperItem > ShaderBuffer::GetHeapWrapperItem() const
{
   return m_shaderResource;
}


void ShaderBuffer::OnDeviceLost()
{
   m_pResource.Reset();
}

void ShaderBuffer::OnDeviceRestored(
   CustomCommandList* const pCommandList,
   ID3D12Device* const pDevice
   )
{
   ShaderTexture2D::UploadCreateResource(
      m_pDrawSystem,
      pCommandList,
      pDevice,
      m_pResource,
      m_shaderResource,
      m_desc,
      m_shaderResourceViewDesc,
      m_data.size(),
      m_data.data(),
      L"Shader Buffer resource"
      );
}
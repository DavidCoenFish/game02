#include "CommonPCH.h"

#include "Common/DrawSystem/Shader/ShaderTexture.h"
#include "Common/DrawSystem/HeapWrapper/HeapWrapperItem.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/d3dx12.h"
#include "Common/DirectXTK12/DirectXHelpers.h"
#include "Common/DirectXTK12/GraphicsMemory.h"

ShaderTexture::ShaderTexture(
   DrawSystem* const pDrawSystem,
   const std::shared_ptr< HeapWrapperItem >& shaderResource,
   const D3D12_RESOURCE_DESC& desc, 
   const D3D12_SHADER_RESOURCE_VIEW_DESC& shaderResourceViewDesc,
   const std::vector<uint8_t>& data
   )
   : IResource( pDrawSystem )
   , m_shaderResource(shaderResource)
   , m_desc(desc)
   , m_shaderResourceViewDesc(shaderResourceViewDesc)
   , m_data(data)
   //, m_bDataDirty(true)
{
   return;
}

void ShaderTexture::OnDeviceLost()
{
   m_pResource.Reset();
}

std::shared_ptr< HeapWrapperItem > ShaderTexture::GetHeapWrapperItem() const
{
   return m_shaderResource;
}

void ShaderTexture::UploadCreateResource(
   DrawSystem* const pDrawSystem,
   ID3D12GraphicsCommandList* const pCommandList,
   ID3D12Device* const pDevice,
   Microsoft::WRL::ComPtr<ID3D12Resource>& pResource,
   std::shared_ptr< HeapWrapperItem >& shaderResource,
   const D3D12_RESOURCE_DESC& desc,
   const D3D12_SHADER_RESOURCE_VIEW_DESC& shaderResourceViewDesc,
   const size_t dataSize,
   void* pData,
   LPCWSTR pName
   )
{
   CD3DX12_HEAP_PROPERTIES heapDefault(D3D12_HEAP_TYPE_DEFAULT);
   DX::ThrowIfFailed(
      pDevice->CreateCommittedResource(
            &heapDefault,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(pResource.ReleaseAndGetAddressOf())));
   pResource->SetName(pName);

   const UINT64 uploadBufferSize = GetRequiredIntermediateSize(pResource.Get(), 0, 1);
   auto pMemoryUpload = pDrawSystem->AllocateUpload(uploadBufferSize, nullptr, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);

   const int bytesPerTexel = (int)DirectX::BytesPerPixel(desc.Format);

   ////D3D12_ShaderTexture2D_DATA_PITCH_ALIGNMENT 
   D3D12_SUBRESOURCE_DATA ShaderTexture2DData = {};
   ShaderTexture2DData.pData = pData;
   ShaderTexture2DData.RowPitch = desc.Width * bytesPerTexel, 
   ShaderTexture2DData.SlicePitch = dataSize;

   if (pCommandList)
   {
      UpdateSubresources(
         pCommandList,
         pResource.Get(), 
         pMemoryUpload.Resource(), 
         pMemoryUpload.ResourceOffset(), 
         0, 
         1, 
         &ShaderTexture2DData
         );
      auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
      pCommandList->ResourceBarrier(1, &barrier);

   }

   const int frameCount = pDrawSystem->GetBackBufferCount();
   for (int index = 0; index < frameCount; ++index)
   {
      pDevice->CreateShaderResourceView(pResource.Get(), &shaderResourceViewDesc, shaderResource->GetCPUHandleFrame(index));
   }

   return;
}

//std::vector<uint8_t>& ShaderTexture::GetData()
//{
//   m_bDataDirty = true;
//   return m_data;
//}

//void ShaderTexture::WriteData(
//   const int x,
//   const int y,
//   const size_t dataSizeBytes,
//   void* pData
//   )
//{
//   const int bytesPerTexel = (int)DirectX::BytesPerPixel(m_desc.Format);
//   const int pixelCount = (int)dataSizeBytes / bytesPerTexel;
//   if (false == m_bDataDirty)
//   {
//      m_bDataDirty = true;
//      m_dataDirtyBox.left = x;
//      m_dataDirtyBox.right = x + pixelCount;
//      m_dataDirtyBox.top = y;
//      m_dataDirtyBox.bottom = y + 1;
//      m_dataDirtyBox.front = 0;
//      m_dataDirtyBox.back = 1;
//   }
//   else
//   {
//      m_dataDirtyBox.left = std::min<UINT>(x, m_dataDirtyBox.left);
//      m_dataDirtyBox.right = std::max<UINT>(x + pixelCount, m_dataDirtyBox.right);
//
//      m_dataDirtyBox.top = std::min<UINT>(y, m_dataDirtyBox.top);
//      m_dataDirtyBox.bottom = std::max<UINT>(y + 1, m_dataDirtyBox.bottom);
//   }
//
//   memcpy( &m_data[(y * m_desc.Width) + x], pData, dataSizeBytes);
//
//   return;
//}


//void ShaderTexture::UploadChangesIfNeeded(
//   ID3D12GraphicsCommandList* const pCommandList
//   )
//{
//   if (false == m_bDataDirty)
//   {
//      return;
//   }
//   m_bDataDirty = false;
//
//   pCommandList->ResourceBarrier(m_pResource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
//
//#if 0
//   const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_pResource.Get(), 0, 1);
//   auto pMemoryUpload = m_pDrawSystem->AllocateUpload(uploadBufferSize, nullptr, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
//
//   const int bytesPerTexel = (int)DirectX::BytesPerPixel(m_desc.Format);
//
//   ////D3D12_ShaderTexture2D_DATA_PITCH_ALIGNMENT 
//   D3D12_SUBRESOURCE_DATA ShaderTexture2DData = {};
//   ShaderTexture2DData.pData = m_data.data();
//   ShaderTexture2DData.RowPitch = m_desc.Width * bytesPerTexel, 
//   ShaderTexture2DData.SlicePitch = m_data.size();
//
//   pCommandList->UpdateSubresourcesMethod(
//      m_pResource.Get(), 
//      pMemoryUpload.Resource(), 
//      pMemoryUpload.ResourceOffset(), 
//      0, 
//      1, 
//      &ShaderTexture2DData
//      );
//#else
//   const UINT64 rowSizeBytes = GetRowSizeBytes(m_pResource.Get(), 0, 1);
//   const UINT rowCount = m_dataDirtyBox.bottom - m_dataDirtyBox.top;
//   auto pMemoryUpload = m_pDrawSystem->AllocateUpload(rowSizeBytes * rowCount, nullptr, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
//
//   const int bytesPerTexel = (int)DirectX::BytesPerPixel(m_desc.Format);
//
//   // take a window of the total data, keeping the same width as the source data
//   D3D12_SUBRESOURCE_DATA ShaderTexture2DData = {};
//   ShaderTexture2DData.pData = &m_data[m_dataDirtyBox.top * m_desc.Width];
//   ShaderTexture2DData.RowPitch = m_desc.Width * bytesPerTexel, 
//   ShaderTexture2DData.SlicePitch = rowSizeBytes * rowCount;
//
//   pCommandList->UpdateSubresourcesMethod(
//      m_pResource.Get(), 
//      pMemoryUpload.Resource(), 
//      pMemoryUpload.ResourceOffset(), 
//      0, 
//      1, 
//      &ShaderTexture2DData,
//      &m_dataDirtyBox
//      );
//
//#endif
//
//   pCommandList->ResourceBarrier(m_pResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
//}

void ShaderTexture::OnDeviceRestored(
   ID3D12GraphicsCommandList* const pCommandList,
   ID3D12Device* const pDevice
   )
{
   UploadCreateResource(
      m_pDrawSystem,
      pCommandList,
      pDevice,
      m_pResource,
      m_shaderResource,
      m_desc,
      m_shaderResourceViewDesc,
      m_data.size(),
      m_data.data(),
      L"Shader Texture2D resource"
      );
   //m_bDataDirty = false;

   return;
}


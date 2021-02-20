#include "CommonPCH.h"

#include "Common/Render/Geometry.h"
#include "Common/Render/DrawSystem.h"
#include "Common/Render/CustomCommandList.h"
#include "Common/DirectXTK12/GraphicsMemory.h"

Geometry::Geometry(
   DrawSystem* const pDrawSystem,
   const D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
   const std::vector< D3D12_INPUT_ELEMENT_DESC >& inputElementDescArray,
   const int floatPerVertex,
   const std::vector< float >& vertexRawData
   )
   : iResource(pDrawSystem)
   , m_primitiveTopology(primitiveTopology)
   , m_inputElementDescArray(inputElementDescArray)
   , m_floatPerVertex(floatPerVertex)
   , m_vertexRawData(vertexRawData)
   , m_vertexBuffer()
   , m_vertexBufferView()
{
}

Geometry::~Geometry()
{
   return;
}

void Geometry::Draw(ID3D12GraphicsCommandList* const pCommandList)
{
   PIXBeginEvent(pCommandList, PIX_COLOR_DEFAULT, L"Draw");

   pCommandList->IASetPrimitiveTopology(m_primitiveTopology);
   pCommandList->IASetVertexBuffers(
      0, 
      1, 
      &m_vertexBufferView
      );

   pCommandList->DrawInstanced(
      (UINT)m_vertexRawData.size() / m_floatPerVertex,
      1, 
      0, 
      0
      );

   PIXEndEvent(pCommandList);
}

void Geometry::OnDeviceLost()
{
   m_vertexBuffer.Reset();
}

void Geometry::OnDeviceRestored(
   CustomCommandList* const pCommandList,
   ID3D12Device* const pDevice
   )
{
   const int byteTotalSize = (int)(sizeof(float) * m_vertexRawData.size());
   auto bufferResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(byteTotalSize);

   auto pUploadMemory = m_pDrawSystem->AllocateUpload(byteTotalSize);

   //create memory on gpu for vertex buffer
   {
      auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
      pDevice->CreateCommittedResource(
         &heapProperties, // a default heap
         D3D12_HEAP_FLAG_NONE, // no flags
         &bufferResourceDesc, // resource description for a buffer
         D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
                                          // from the upload heap to this heap
         nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
         IID_PPV_ARGS(m_vertexBuffer.ReleaseAndGetAddressOf())
         );
      m_vertexBuffer->SetName(L"GeometryVertexBuffer");
   }

   {
      if (pCommandList)
      {
         D3D12_SUBRESOURCE_DATA vertexData = {};
         vertexData.pData = m_vertexRawData.data();
         vertexData.RowPitch = byteTotalSize;
         vertexData.SlicePitch = byteTotalSize;

         pCommandList->UpdateSubresourcesMethod(
            m_vertexBuffer.Get(), 
            pUploadMemory.Resource(), 
            pUploadMemory.ResourceOffset(), 
            0, 
            1, 
            &vertexData
            );
      }

      pCommandList->ResourceBarrier(m_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
   }

   m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
   m_vertexBufferView.StrideInBytes = sizeof(float) * m_floatPerVertex;
   m_vertexBufferView.SizeInBytes = byteTotalSize;

   return;
}

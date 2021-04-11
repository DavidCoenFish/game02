#include "CommonPCH.h"

#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/DeviceResources.h"
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/DrawSystem/CustomCommandList.h"
#include "Common/DrawSystem/HeapWrapper/HeapWrapper.h"
#include "Common/DrawSystem/HeapWrapper/HeapWrapperItem.h"
#include "Common/DrawSystem/Shader/Shader.h"
#include "Common/DrawSystem/Geometry/GeometryGeneric.h"
#include "Common/DirectXTK12/GraphicsMemory.h"

DrawSystem::DrawSystem(
   const HWND hWnd,
   const unsigned int backBufferCount,
   const D3D_FEATURE_LEVEL d3dFeatureLevel,
   const unsigned int options
   )
   : m_hWnd(hWnd)
   , m_backBufferCount(backBufferCount)
   , m_d3dFeatureLevel(d3dFeatureLevel)
   , m_options(options)
{
   m_pHeapWrapperCbvSrvUav = std::make_shared< HeapWrapper >( this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, backBufferCount, true );
   m_pHeapWrapperSampler = std::make_shared< HeapWrapper >( this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER );
   m_pHeapWrapperRenderTargetView = std::make_shared< HeapWrapper >( this, D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
   m_pHeapWrapperDepthStencilView = std::make_shared< HeapWrapper >( this, D3D12_DESCRIPTOR_HEAP_TYPE_DSV );

   CreateDeviceResources();
}

DrawSystem::~DrawSystem()
{
   WaitForGpu();
   for (auto pIter : m_listResource)
   {
      pIter->OnDeviceLost();
   }
   m_listResource.clear();
   m_pDeviceResources.reset();
}

DirectX::GraphicsResource DrawSystem::AllocateConstant(const std::size_t size, void* const pConstants)
{
   return AllocateUpload(size, pConstants, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
}

DirectX::GraphicsResource DrawSystem::AllocateUpload(
   const std::size_t size, 
   void* const pDataOrNullptr, 
   size_t alignment
   )
{
   if (m_pDeviceResources)
   {
      return m_pDeviceResources->AllocateUpload(size, pDataOrNullptr, alignment);
   }
   return DirectX::GraphicsResource();
}

std::shared_ptr< Shader > DrawSystem::MakeShader(
   ID3D12GraphicsCommandList* const pCommandList,
   const ShaderPipelineStateData& pipelineStateData,
   const std::shared_ptr< std::vector<uint8_t> >& pVertexShaderData,
   const std::shared_ptr< std::vector<uint8_t> >& pGeometryShaderData,
   const std::shared_ptr< std::vector<uint8_t> >& pPixelShaderData,
   const std::vector< std::shared_ptr< ShaderResourceInfo > >& arrayShaderResourceInfo,
   const std::vector< std::shared_ptr< ShaderConstantInfo > >& arrayShaderConstantsInfo
   )
{
   auto pResult = std::make_shared<Shader>(
      this,
      pipelineStateData,
      pVertexShaderData,
      pGeometryShaderData,
      pPixelShaderData,
      arrayShaderResourceInfo,
      arrayShaderConstantsInfo
      );
   if (pResult && m_pDeviceResources)
   {
      ((IResource*)(pResult.get()))->OnDeviceRestored(
         pCommandList,
         m_pDeviceResources->GetD3dDevice()
         );
   }
   return pResult;
}

std::shared_ptr< GeometryGeneric > DrawSystem::MakeGeometryGeneric(
   ID3D12GraphicsCommandList* const pCommandList,
   const D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
   const std::vector< D3D12_INPUT_ELEMENT_DESC >& inputElementDescArray,
   const std::vector< float >& vertexDataRaw,
   const int floatPerVertex
   )
{
   auto pResult = std::make_shared<GeometryGeneric>(
      this,
      primitiveTopology,
      inputElementDescArray,
      vertexDataRaw,
      floatPerVertex
      );
   if (pResult && m_pDeviceResources)
   {
      ((IResource*)(pResult.get()))->OnDeviceRestored(
         pCommandList,
         m_pDeviceResources->GetD3dDevice()
         );
   }
   return pResult;
}

std::shared_ptr<CustomCommandList> DrawSystem::CreateCustomCommandList()
{
   if (m_pDeviceResources)
   {
      auto pCommandList = m_pDeviceResources->GetCustomCommandList();
      return std::make_shared< CustomCommandList>(
         *this,
         pCommandList
         );
   }
   return nullptr;
}

void DrawSystem::CustomCommandListFinish(ID3D12GraphicsCommandList* pCommandList)
{
   if (m_pDeviceResources)
   {
      m_pDeviceResources->CustomCommandListFinish(pCommandList);
   }
   return;
}

std::unique_ptr< DrawSystemFrame > DrawSystem::CreateNewFrame()
{
   return std::make_unique< DrawSystemFrame >( *this );
}

void DrawSystem::Prepare(
   ID3D12GraphicsCommandList*& pCommandList
   )
{
   if (nullptr != m_pDeviceResources)
   {
       m_pDeviceResources->Prepare(
          pCommandList
          );
   }

   return;
}

//void DrawSystem::Clear()
//{
//   if (nullptr == m_pDeviceResources)
//   {
//      return;
//   }
//   m_pDeviceResources->Clear();
//}

void DrawSystem::Present()
{
   if (nullptr == m_pDeviceResources)
   {
      return;
   }

   if (false == m_pDeviceResources->Present())
   {
      CreateDeviceResources();
   }
}

IRenderTarget* DrawSystem::GetRenderTargetBackBuffer()
{
   if (m_pDeviceResources)
   {
      return m_pDeviceResources->GetRenderTargetBackBuffer();
   }
   return nullptr;
}

std::shared_ptr<HeapWrapperItem> DrawSystem::MakeHeapWrapperCbvSrvUav(const int length)
{
   return HeapWrapperItem::Factory( 
      m_pDeviceResources ? m_pDeviceResources->GetD3dDevice() : nullptr,
      m_pHeapWrapperCbvSrvUav, 
      length 
      );
}
std::shared_ptr<HeapWrapperItem> DrawSystem::MakeHeapWrapperSampler(const int length)
{
   return HeapWrapperItem::Factory( 
      m_pDeviceResources ? m_pDeviceResources->GetD3dDevice() : nullptr,
      m_pHeapWrapperSampler, 
      length 
      );
}
std::shared_ptr<HeapWrapperItem> DrawSystem::MakeHeapWrapperRenderTargetView(const int length)
{
   return HeapWrapperItem::Factory( 
      m_pDeviceResources ? m_pDeviceResources->GetD3dDevice() : nullptr,
      m_pHeapWrapperRenderTargetView, 
      length 
      );
}
std::shared_ptr<HeapWrapperItem> DrawSystem::MakeHeapWrapperDepthStencilView(const int length)
{
   return HeapWrapperItem::Factory( 
      m_pDeviceResources ? m_pDeviceResources->GetD3dDevice() : nullptr,
      m_pHeapWrapperDepthStencilView, 
      length 
      );
}

void DrawSystem::WaitForGpu() noexcept
{
   if (m_pDeviceResources)
   {
      m_pDeviceResources->WaitForGpu();
   }
}

void DrawSystem::OnResize()
{
   if (m_pDeviceResources)
   {
      m_pDeviceResources->OnResize(this, m_hWnd);
   }
}

void DrawSystem::AddResource(IResource* const pResource)
{
   if (pResource)
   {
      m_listResource.push_back( pResource );
   }
}

void DrawSystem::RemoveResource(IResource* const pResource)
{
   //if RemoveResource is called by the dtor, then not a good idea to call a virutal func on the pResource?
   //if (pResource)
   //{
   //   pResource->OnDeviceLost();
   //}

   m_listResource.remove( pResource );
}

const int DrawSystem::GetBackBufferIndex() const
{
   if (nullptr != m_pDeviceResources)
   {
      m_pDeviceResources->GetBackBufferIndex();
   }
   return 0;
}


void DrawSystem::CreateDeviceResources()
{
   for (auto pIter : m_listResource)
   {
      pIter->OnDeviceLost();
   }
   m_pDeviceResources.reset();
   m_pDeviceResources = std::make_unique< DeviceResources >(
      2,
      m_d3dFeatureLevel,
      m_options
      );
   //two pass construction as rendertargetbackbuffer calls MakeHeapWrapperRenderTargetView, MakeHeapWrapperDepthStencilView which need m_pDeviceResources assigned
   m_pDeviceResources->CreateWindowSizeDependentResources(this, m_hWnd);

   if (0 < m_listResource.size())
   {
      auto pCommandList = CreateCustomCommandList();
      for (auto pIter : m_listResource)
      {
         pIter->OnDeviceRestored(
            pCommandList->GetCommandList(),
            m_pDeviceResources->GetD3dDevice()
            );
      }
   }
}



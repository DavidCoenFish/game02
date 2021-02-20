#include "CommonPCH.h"

#include "Common/Render/DrawSystem.h"
#include "Common/Render/DrawSystemResources.h"
#include "Common/Render/DrawSystemResourcesSize.h"
#include "Common/Render/iResource.h"
#include "Common/Render/HeapWrapper.h"
#include "Common/Render/HeapWrapperItem.h"
#include "Common/Render/Geometry.h"
#include "Common/Render/Shader.h"
#include "Common/Render/ShaderTexture2D.h"
#include "Common/Render/ShaderBuffer.h"
#include "Common/Render/RenderTargetTexture.h"
#include "Common/DirectXTK12/GraphicsMemory.h"

DrawSystem::DrawSystem(
   const std::function< void(void) >& callbackFatalExit,
   HWND hwnd, 
   const int width, 
   const int height,
   const int backBufferCount,
   const RenderTargetFormatData& targetFormatData,
   const RenderTargetDepthData& targetDepthData,
   const unsigned int flags,
   const D3D_FEATURE_LEVEL d3dMinFeatureLevel
   )
   : m_hwnd(hwnd)
   , m_flags(flags)
   , m_d3dMinFeatureLevel(d3dMinFeatureLevel)
   , m_backBufferWidth(width)
   , m_backBufferHeight(height)
   , m_backBufferCount(backBufferCount)
   , m_backBufferData(targetFormatData)
   , m_backDepthData(targetDepthData)
   , m_deviceLostLoopCount(0)
   , m_callbackFatalExit(callbackFatalExit)
{
   m_pHeapWrapperCbvSrvUav = std::make_shared< HeapWrapper >( this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, backBufferCount, true );
   m_pHeapWrapperSampler = std::make_shared< HeapWrapper >( this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER );
   m_pHeapWrapperRenderTargetView = std::make_shared< HeapWrapper >( this, D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
   m_pHeapWrapperDepthStencilView = std::make_shared< HeapWrapper >( this, D3D12_DESCRIPTOR_HEAP_TYPE_DSV );

   CreateDevice();

   return;
}

DrawSystem::~DrawSystem()
{
   WaitForGpu();
   //release the resources, else the m_pHeapWrapperXXX could still be holding device resources
   for (auto pIter : m_listResource)
   {
      pIter->OnDeviceLost();
   }
   WaitForGpu();

   m_pHeapWrapperCbvSrvUav = nullptr;
   m_pHeapWrapperSampler = nullptr;
   m_pHeapWrapperRenderTargetView = nullptr;
   m_pHeapWrapperDepthStencilView = nullptr;

   m_pResources.reset();
   //WaitForGpu();
}

//void DrawSystem::SetWindow(HWND window, int width, int height)
//{
//    m_hwnd = window;
//    m_backBufferWidth = width;
//    m_backBufferHeight = height;
//
//    CreateDevice();
//}

void DrawSystem::CreateDeviceResources()
{
   m_pResources.reset();
   m_pResources = std::make_unique< DrawSystemResources >( 
      this,
      m_flags, 
      m_d3dMinFeatureLevel,
      m_backBufferCount,
      m_backBufferData,
      m_backDepthData
      );
}

static DXGI_FORMAT NoSRGB(DXGI_FORMAT fmt)
{
   switch (fmt)
   {
   case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:   return DXGI_FORMAT_R8G8B8A8_UNORM;
   case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8A8_UNORM;
   case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8X8_UNORM;
   default:                                return fmt;
   }
}

void DrawSystem::CreateWindowSizeDependentResources()
{
   if (m_pResources)
   {
      DXGI_FORMAT backBufferFormat = NoSRGB(m_backBufferData.format);
      HRESULT hr = m_pResources->CreateWindowSizeDependentResources(
         m_backBufferCount,
         m_backBufferWidth,
         m_backBufferHeight,
         backBufferFormat,
         m_flags,
         m_hwnd
         );
      if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
      {
#ifdef _DEBUG
         char buff[64] = {};
         sprintf_s(buff, "Device Lost on ResizeBuffers: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? m_pResources->GetDeviceRemovedReason() : hr);
         OutputDebugStringA(buff);
#endif
         m_deviceLostLoopCount += 1;
         if ((5 < m_deviceLostLoopCount) && m_callbackFatalExit)
         {
            m_callbackFatalExit();
            return;
         }
         // If the device was removed for any reason, a new device and swap chain will need to be created.
         HandleDeviceLost();

         // Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method
         // and correctly set up the new device.
      }
   }
   m_deviceLostLoopCount = 0;
}

void DrawSystem::CreateDevice()
{
   CreateDeviceResources();

   CreateWindowSizeDependentResources();

   {
      auto pCommandList = MakeCommandList();
      for (auto pIter : m_listResource)
      {
         DeviceRestored( pIter, pCommandList.get() );
      }
   }
   WaitForGpu();
}

void DrawSystem::HandleDeviceLost()
{
   WaitForGpu();

   for (auto pIter : m_listResource)
   {
      pIter->OnDeviceLost();
   }
   m_pResources.reset();

   CreateDevice();
}

bool DrawSystem::WindowSizeChanged(int width, int height)
{
   if ((m_backBufferWidth == width) &&
      (m_backBufferHeight == height))
   {
      return false;
   }
   m_backBufferWidth = width;
   m_backBufferHeight = height;

   WaitForGpu();

   for (auto pIter : m_listResource)
   {
      pIter->OnResizeBefore();
   }

   CreateWindowSizeDependentResources();

   if (m_pResources)
   {
      {
         auto pCommandList = MakeCommandList();
         m_pResources->OnWindowSizeChanged(pCommandList.get());

         for (auto pIter : m_listResource)
         {
            pIter->OnResizeAfter(
               pCommandList.get(),
               m_pResources->GetD3dDevice(),
               width,
               height
               );
         }
      }

      WaitForGpu();
   }

   return true;
}

std::shared_ptr< CustomCommandList > DrawSystem::MakeCommandList()
{
   if (m_pResources)
   {
      return m_pResources->MakeCommandList();
   }
   return nullptr;
}

iRenderTarget* const DrawSystem::GetCurrentRenderTargetBackbuffer()
{
   if (m_pResources)
   {
      return m_pResources->GetCurrentRenderTargetBackbuffer();
   }
   return nullptr;
}

void DrawSystem::Present()
{
   if (m_pResources)
   {
      const HRESULT hr = m_pResources->Present( m_flags );
      if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
      {
   #ifdef _DEBUG
         char buff[64] = {};
         sprintf_s(buff, "Device Lost on Present: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? m_pResources->GetDeviceRemovedReason() : hr);
         OutputDebugStringA(buff);
   #endif
         HandleDeviceLost();
      }
   }
}

// Wait for pending GPU work to complete.
void DrawSystem::WaitForGpu() noexcept
{
   if (nullptr != m_pResources)
   {
      m_pResources->WaitForGpu();
   }
   return;
}

void DrawSystem::AddResource(iResource* const pResource)
{
   if (pResource)
   {
      m_listResource.push_back( pResource );

      //if doing AddResource on ctor, don't call virtual functions...?
      //if (m_pResources)
      //{
      //   m_pResources->DeviceRestored( pResource );
      //}
   }
}
void DrawSystem::RemoveResource(iResource* const pResource)
{
   m_listResource.remove( pResource );
}

std::shared_ptr< Geometry > DrawSystem::GeometryFactory( 
   CustomCommandList* const pCommandList,
   const D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
   const std::vector< D3D12_INPUT_ELEMENT_DESC >& inputElementDescArray,
   const int floatPerVertex,
   const std::vector< float >& vertexRawData
   )
{
   auto pResult = std::make_shared< Geometry >( 
      this,
      primitiveTopology,
      inputElementDescArray,
      floatPerVertex,
      vertexRawData
      );
   DeviceRestored( pResult.get(), pCommandList );
   return pResult;
}

std::shared_ptr< Shader > DrawSystem::ShaderFactory( 
   CustomCommandList* const pCommandList,
   const ShaderPipelineStateData& pipelineStateData,
   const std::shared_ptr< std::vector<uint8_t> >& pVertexShaderData,
   const std::shared_ptr< std::vector<uint8_t> >& pGeometryShaderData,
   const std::shared_ptr< std::vector<uint8_t> >& pPixelShaderData,
   const std::vector< std::shared_ptr< ShaderResourceInfo > >& arrayShaderResourceInfo,
   const std::vector< std::shared_ptr< ShaderConstantInfo > >& arrayShaderConstantsInfo
   )
{
   auto pResult = std::make_shared< Shader >( 
      this,
      pipelineStateData,
      pVertexShaderData,
      pGeometryShaderData,
      pPixelShaderData,
      arrayShaderResourceInfo,
      arrayShaderConstantsInfo
      );
   DeviceRestored( pResult.get(), pCommandList );
   return pResult;
}

std::shared_ptr< ShaderTexture2D > DrawSystem::TextureFactoryTest(
   CustomCommandList* const pCommandList
   )
{
   auto pShaderResource = MakeHeapWrapperCbvSrvUav();

   D3D12_RESOURCE_DESC desc = {
       D3D12_RESOURCE_DIMENSION_TEXTURE2D, //D3D12_RESOURCE_DIMENSION Dimension;
       D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, //UINT64 Alignment;
       256, //UINT64 Width;
       256, //UINT Height;
       1, //UINT16 DepthOrArraySize;
       1, //UINT16 MipLevels;
       DXGI_FORMAT_B8G8R8A8_UNORM, //DXGI_FORMAT Format;
       DXGI_SAMPLE_DESC{ 1, 0 }, //DXGI_SAMPLE_DESC SampleDesc;
       D3D12_TEXTURE_LAYOUT_UNKNOWN, //D3D12_TEXTURE_LAYOUT Layout;
       D3D12_RESOURCE_FLAG_NONE //D3D12_RESOURCE_FLAGS Flags;
   };

   std::vector<uint8_t> data;
   for (int y = 0; y < 256; ++y)
   {
      for (int x = 0; x < 256; ++x)
      {
         uint8_t red = (0 != (y & 0x10)) == (0 != (x & 0x10)) ? 255 : 0;
         data.push_back((uint8_t)x); //blue
         data.push_back((uint8_t)y); //green
         data.push_back(red); //red
         data.push_back(255); //alpha
      }
   }

   auto pResult = InternalTextureFactory(
      pCommandList,
      pShaderResource,
      desc,
      data
      );

   return pResult;
}

std::shared_ptr< ShaderTexture2D > DrawSystem::TextureFactory(
   CustomCommandList* const pCommandList,
   const int width, 
   const int height,
   const DXGI_FORMAT format,
   const std::vector<uint8_t>& data,
   const int mipLevels
   )
{
   auto pShaderResource = MakeHeapWrapperCbvSrvUav();

   D3D12_RESOURCE_DESC desc = {
      D3D12_RESOURCE_DIMENSION_TEXTURE2D, //D3D12_RESOURCE_DIMENSION Dimension;
      D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, //UINT64 Alignment;
      (UINT64)width, //UINT64 Width;
      (UINT)height, //UINT Height;
      (UINT16)1, //UINT16 DepthOrArraySize;
      (UINT16)mipLevels, //1, //UINT16 MipLevels;
      format, //DXGI_FORMAT Format;
      DXGI_SAMPLE_DESC{ 1, 0 }, //DXGI_SAMPLE_DESC SampleDesc;
      D3D12_TEXTURE_LAYOUT_UNKNOWN, //D3D12_TEXTURE_LAYOUT Layout;
      D3D12_RESOURCE_FLAG_NONE //D3D12_RESOURCE_FLAGS Flags;
      };

   auto pResult = InternalTextureFactory(
      pCommandList,
      pShaderResource,
      desc,
      data
      );

   return pResult;
}

//Xbox-ATG-Samples-master\PCSamples\Raytracing\RaytracingAO_PC12\AO.cpp void AO::SetMesh(std::shared_ptr<Mesh> pMesh)
//TextureFactoryBuffer32bit
//DXGI_FORMAT_R32_TYPELESS
//D3D12_BUFFER_SRV_FLAG_RAW
//StructureByteStride = 0;


std::shared_ptr< ShaderTexture2D > DrawSystem::InternalTextureFactory(
   CustomCommandList* const pCommandList,
   const std::shared_ptr< HeapWrapperItem >& pShaderResource,
   const D3D12_RESOURCE_DESC& desc, 
   const std::vector<uint8_t>& data,
   const UINT mipLevels
   )
{
   // Describe and create a SRV for the texture.
   D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
   srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
   srvDesc.Format = desc.Format;
   srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
   srvDesc.Texture2D.MipLevels = mipLevels;

   auto pResult = std::make_shared< ShaderTexture2D >(
      this,
      pShaderResource,
      desc,
      srvDesc,
      data
      );
   DeviceRestored( pResult.get(), pCommandList );
   return pResult;
}

std::shared_ptr< ShaderBuffer > DrawSystem::FactoryShaderBuffer(
   CustomCommandList* const pCommandList,
   const UINT firstElement,
   const int floatsPerElement,
   const std::vector<float>& data
   )
{
   auto pShaderResource = MakeHeapWrapperCbvSrvUav();

   const UINT64 numElements = data.size() / floatsPerElement;
   const UINT64 elementSize = sizeof(float) * floatsPerElement;
   auto desc = CD3DX12_RESOURCE_DESC::Buffer(numElements * elementSize);

   D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
   srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
   srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
   srvDesc.Format = DXGI_FORMAT_UNKNOWN;
   srvDesc.Buffer.NumElements = (UINT)numElements;
   srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
   srvDesc.Buffer.StructureByteStride = (UINT)elementSize;
   srvDesc.Buffer.FirstElement = firstElement;

   auto pResult = std::make_shared< ShaderBuffer >(
      this,
      pShaderResource,
      desc,
      srvDesc,
      data
      );
   DeviceRestored( pResult.get(), pCommandList );
   return pResult;
}

std::shared_ptr< RenderTargetTexture > DrawSystem::RenderTargetFactory(
   CustomCommandList* const pCommandList,
   const std::vector< RenderTargetFormatData >& targetFormatDataArray,
   const RenderTargetDepthData& targetDepthData,
   const int width,
   const int height,
   const bool bResizeWithScreen
   )
{
   auto pResult = std::make_shared< RenderTargetTexture >(
      this,
      targetFormatDataArray,
      targetDepthData,
      width,
      height,
      bResizeWithScreen
      );
   DeviceRestored( pResult.get(), pCommandList );
   return pResult;
}

std::shared_ptr<HeapWrapperItem> DrawSystem::MakeHeapWrapperCbvSrvUav(const int length)
{
   return HeapWrapperItem::Factory( 
      m_pResources ? m_pResources->GetD3dDevice() : nullptr,
      m_pHeapWrapperCbvSrvUav, 
      length 
      );
}
std::shared_ptr<HeapWrapperItem> DrawSystem::MakeHeapWrapperSampler(const int length)
{
   return HeapWrapperItem::Factory( 
      m_pResources ? m_pResources->GetD3dDevice() : nullptr,
      m_pHeapWrapperSampler, 
      length 
      );
}
std::shared_ptr<HeapWrapperItem> DrawSystem::MakeHeapWrapperRenderTargetView(const int length)
{
   return HeapWrapperItem::Factory( 
      m_pResources ? m_pResources->GetD3dDevice() : nullptr,
      m_pHeapWrapperRenderTargetView, 
      length 
      );
}
std::shared_ptr<HeapWrapperItem> DrawSystem::MakeHeapWrapperDepthStencilView(const int length)
{
   return HeapWrapperItem::Factory( 
      m_pResources ? m_pResources->GetD3dDevice() : nullptr,
      m_pHeapWrapperDepthStencilView, 
      length 
      );
}

void DrawSystem::GetBackBufferWidthHeight(int& width, int& height)
{
   width = m_backBufferWidth;
   height = m_backBufferHeight;
}

DirectX::GraphicsResource DrawSystem::AllocateConstant(const std::size_t size, void* const pConstants)
{
   return AllocateUpload(size, pConstants, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
}

DirectX::GraphicsResource DrawSystem::AllocateUpload(const std::size_t size, void* const pDataOrNullptr, size_t alignment)
{
   if (m_pResources)
   {
      return m_pResources->AllocateUpload(size, pDataOrNullptr, alignment);
   }
   return DirectX::GraphicsResource();
}

const int DrawSystem::GetBackBufferIndex() const
{
   if (m_pResources)
   {
      return m_pResources->GetBackBufferIndex();
   }
   return 0;
}

IDXGISwapChain3* DrawSystem::GetSwapChain() const
{
   if (m_pResources)
   {
      return m_pResources->GetSwapChain();
   }
   return nullptr;
}

void DrawSystem::DeviceRestored( 
   iResource* const pResource,
   CustomCommandList* const pCommandList
   )
{
   if (nullptr != m_pResources)
   {
      m_pResources->DeviceRestored( pResource, pCommandList );
   }
   return;
}

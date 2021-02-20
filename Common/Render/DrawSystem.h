#pragma once

#include "Common/Render/RenderTargetDepthData.h"
#include "Common/Render/RenderTargetFormatData.h"

class CustomCommandList;
class DrawSystemResources;
class DrawSystemResourcesSize;
class Geometry;
class HeapWrapper;
class HeapWrapperItem;
class iResource;
class iRenderTarget;
class RenderTargetTexture;
class RenderTargetBackBuffer;
class Shader;
class ShaderBuffer;
class ShaderTexture2D;
class ShaderResourceInfo;
class ShaderConstantInfo;
namespace DirectX
{
   class GraphicsMemory;
   class GraphicsResource;
};
struct ShaderPipelineStateData;

class DrawSystem
{
public:
   static const unsigned int c_AllowTearing    = 0x1;
   static const unsigned int c_EnableHDR       = 0x2;

   explicit DrawSystem(
      const std::function< void(void) >& callbackFatalExit,
      HWND hwnd, 
      const int width, 
      const int height,
      const int backBufferCount = 2,
      const RenderTargetFormatData& targetFormatData = RenderTargetFormatData(),
      const RenderTargetDepthData& targetDepthData = RenderTargetDepthData(),
      const unsigned int flags = 0,
      const D3D_FEATURE_LEVEL d3dMinFeatureLevel = D3D_FEATURE_LEVEL_11_0
      );
   ~DrawSystem();

   //void SetWindow(HWND window, int width, int height);
   void HandleDeviceLost();

   bool WindowSizeChanged(int width, int height);

   std::shared_ptr< CustomCommandList > MakeCommandList();
   iRenderTarget* const GetCurrentRenderTargetBackbuffer();

   void Present();

   void WaitForGpu() noexcept;

   void AddResource(iResource* const pResource);
   void RemoveResource(iResource* const pResource);

   std::shared_ptr< Geometry > GeometryFactory( 
      CustomCommandList* const pCommandList,
      const D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
      const std::vector< D3D12_INPUT_ELEMENT_DESC >& inputElementDescArray,
      const int floatPerVertex,
      const std::vector< float >& vertexRawData
      );

   std::shared_ptr< Shader > ShaderFactory( 
      CustomCommandList* const pCommandList,
      const ShaderPipelineStateData& pipelineStateData,
      const std::shared_ptr< std::vector<uint8_t> >& pVertexShaderData,
      const std::shared_ptr< std::vector<uint8_t> >& pGeometryShaderData,
      const std::shared_ptr< std::vector<uint8_t> >& pPixelShaderData,
      const std::vector< std::shared_ptr< ShaderResourceInfo > >& arrayShaderResourceInfo = std::vector< std::shared_ptr< ShaderResourceInfo > >(),
      const std::vector< std::shared_ptr< ShaderConstantInfo > >& arrayShaderConstantsInfo = std::vector< std::shared_ptr< ShaderConstantInfo > >()
      );

   std::shared_ptr< ShaderTexture2D > TextureFactoryTest(
      CustomCommandList* const pCommandList
      );
   std::shared_ptr< ShaderTexture2D > TextureFactory(
      CustomCommandList* const pCommandList,
      const int width, 
      const int height,
      const DXGI_FORMAT format,
      const std::vector<uint8_t>& data,
      const int mipLevels = 1
      );
   std::shared_ptr< ShaderBuffer > FactoryShaderBuffer(
      CustomCommandList* const pCommandList,
      const UINT firstElement,
      const int floatsPerElement, //[1,2,3,4]
      const std::vector<float>& data
      );

   std::shared_ptr< RenderTargetTexture > RenderTargetFactory(
      CustomCommandList* const pCommandList,
      const std::vector< RenderTargetFormatData >& targetFormatDataArray,
      const RenderTargetDepthData& targetDepthData,
      const int width,
      const int height,
      const bool bResizeWithScreen = false
      );

   std::shared_ptr<HeapWrapperItem> MakeHeapWrapperCbvSrvUav(const int length = 1);
   std::shared_ptr<HeapWrapperItem> MakeHeapWrapperSampler(const int length = 1);
   std::shared_ptr<HeapWrapperItem> MakeHeapWrapperRenderTargetView(const int length = 1);
   std::shared_ptr<HeapWrapperItem> MakeHeapWrapperDepthStencilView(const int length = 1);

   void GetBackBufferWidthHeight(int& width, int& height);

   DirectX::GraphicsResource AllocateConstant(const std::size_t size, void* const pConstants);
   DirectX::GraphicsResource AllocateUpload(
      const std::size_t size, 
      void* const pDataOrNullptr = nullptr, 
      size_t alignment = 16
      );

   const int GetBackBufferIndex() const;
   const int GetBackBufferCount() const { return m_backBufferCount; }
   IDXGISwapChain3* GetSwapChain() const;// m_swapChain;

   void DeviceRestored( 
      iResource* const pResource,
      CustomCommandList* const pCommandList
      );

private:
   void CreateDevice();
   void CreateDeviceResources();
   void CreateWindowSizeDependentResources();

   std::shared_ptr< ShaderTexture2D > InternalTextureFactory(
      CustomCommandList* const pCommandList,
      const std::shared_ptr< HeapWrapperItem >& pShaderResource,
      const D3D12_RESOURCE_DESC& desc, 
      const std::vector<uint8_t>& data,
      const UINT mipLevels = 1
      );

private:
   unsigned int m_flags;
   D3D_FEATURE_LEVEL m_d3dMinFeatureLevel;
   int m_backBufferCount;
   RenderTargetFormatData m_backBufferData;
   RenderTargetDepthData m_backDepthData;

   std::unique_ptr< DrawSystemResources > m_pResources;
   std::list< iResource* > m_listResource;
   std::shared_ptr< HeapWrapper > m_pHeapWrapperCbvSrvUav;
   std::shared_ptr< HeapWrapper > m_pHeapWrapperSampler;
   std::shared_ptr< HeapWrapper > m_pHeapWrapperRenderTargetView;
   std::shared_ptr< HeapWrapper > m_pHeapWrapperDepthStencilView;

   int m_deviceLostLoopCount;
   std::function< void(void) > m_callbackFatalExit;

   // Cached device properties.
   HWND m_hwnd;
   int m_backBufferWidth;
   int m_backBufferHeight;
};

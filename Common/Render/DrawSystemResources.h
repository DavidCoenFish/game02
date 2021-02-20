#pragma once

class DrawSystem;
class DrawSystemResourcesSize;
class iResource;
class iGeometry; 
class iShader;
class iRenderTarget;
class HeapWrapper;
class HeapWrapperItem;
class CustomCommandList;
class RenderTargetBackBuffer;
namespace DirectX
{
   class GraphicsMemory;
   class GraphicsResource;
};
struct RenderTargetFormatData;
struct RenderTargetDepthData;

class DrawSystemResources
{
public:
   DrawSystemResources(
      DrawSystem* const pDrawSystem,
      unsigned int& flags, 
      D3D_FEATURE_LEVEL minFeatureLevel,
      const int backBufferCount,
      const RenderTargetFormatData& targetFormatData,
      const RenderTargetDepthData& targetDepthData
      );
   ~DrawSystemResources();

   HRESULT CreateWindowSizeDependentResources(
      const int backBufferCount,
      const int backBufferWidth,
      const int backBufferHeight,
      const DXGI_FORMAT backBufferFormat,
      const int flags,
      const HWND hWnd
      );

   void WaitForGpu() noexcept;
   std::shared_ptr< CustomCommandList > MakeCommandList();
   HRESULT Present(const int flags);
   HRESULT GetDeviceRemovedReason();

   void DeviceRestored(
      iResource* const pResource,
      CustomCommandList* const pCommandList
      );
   ID3D12Device* const GetD3dDevice(); // m_d3dDevice;
   iRenderTarget* const GetCurrentRenderTargetBackbuffer();

   void OnWindowSizeChanged(
      CustomCommandList* const pCommandList
      );

   DirectX::GraphicsResource AllocateUpload(const std::size_t size, void* const pDataOrNullptr, size_t alignment = 16);

   const int GetBackBufferIndex() const { return m_backBufferIndex; }
   IDXGISwapChain3* GetSwapChain() const;// m_swapChain;

private:
   void GetAdapter(IDXGIAdapter1** ppAdapter, D3D_FEATURE_LEVEL minFeatureLevel);
   void MoveToNextFrame();

private:
   static const size_t MAX_BACK_BUFFER_COUNT = 3;

   int m_backBufferCount;
   DWORD m_dxgiFactoryFlags;
   Microsoft::WRL::ComPtr<IDXGIFactory6> m_dxgiFactory;
   Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;
   Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
   Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
   Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocators[MAX_BACK_BUFFER_COUNT];

   Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
   UINT64 m_fenceValues[MAX_BACK_BUFFER_COUNT];
   Microsoft::WRL::Wrappers::Event m_fenceEvent;

   D3D_FEATURE_LEVEL m_d3dFeatureLevel;
   
   Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
   int m_backBufferIndex;

   std::unique_ptr<DirectX::GraphicsMemory> m_pGraphicsMemory;

   std::shared_ptr< RenderTargetBackBuffer > m_renderTargetBackBufferArray[MAX_BACK_BUFFER_COUNT];

};


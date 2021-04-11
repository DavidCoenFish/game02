#pragma once

//enum class DeviceResourcesOptions
//{
//   None = 0,
//   AllowTearing = 0x1,
//   EnableHDR = 0x2
//};
class IResource;
class ScreenSizeResources;
namespace DirectX
{
   class GraphicsMemory;
   class GraphicsResource;
};

class DeviceResources
{
public:
   static const unsigned int c_AllowTearing    = 0x1;
   static const unsigned int c_EnableHDR       = 0x2;

   DeviceResources(
      const HWND hWnd,
      const unsigned int backBufferCount,
      const D3D_FEATURE_LEVEL d3dFeatureLevel,
      const unsigned int options
      );
   ~DeviceResources();
   void WaitForGpu() noexcept;

   void OnResize(const HWND hWnd);
   const int GetBackBufferIndex() const;
   DirectX::GraphicsResource AllocateUpload(const std::size_t size, void* const pDataOrNullptr, size_t alignment = 16);

   void Prepare(
      ID3D12GraphicsCommandList*& pCommandList
      );
   void Clear();
   const bool Present();

   ID3D12Device* const GetD3dDevice();
   //ID3D12CommandQueue* const GetCommandQueue();

   ID3D12GraphicsCommandList* GetCustomCommandList();
   void CustomCommandListFinish(ID3D12GraphicsCommandList* pCommandList);

   void ApplicationShuttingDown();
private:
   void GetAdapter(IDXGIAdapter1** ppAdapter, const D3D_FEATURE_LEVEL d3dFeatureLevel);
   void CreateWindowSizeDependentResources(
      const HWND hWnd
      );
   void MoveToNextFrame();
   //void UpdateColorSpace();

   void WaitForCustomCommand();

private:
   unsigned int m_backBufferCount;

   unsigned int m_options;
   Microsoft::WRL::ComPtr<IDXGIFactory6> m_pDXGIFactory;
   DWORD m_dxgiFactoryFlags;
   Microsoft::WRL::ComPtr<ID3D12Device> m_pDevice;
   Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_pCommandQueue;

   UINT64 m_customCommandListFenceValue;
   Microsoft::WRL::ComPtr<ID3D12Fence> m_pCustomCommandFence;
   Microsoft::WRL::Wrappers::Event m_customCommandFenceEvent;
   Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_pCustomCommandAllocator;
   Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pCustomCommandList;

   std::unique_ptr< ScreenSizeResources > m_pScreenSizeResources;
   std::unique_ptr<DirectX::GraphicsMemory> m_pGraphicsMemory;

};

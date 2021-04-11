#pragma once

struct CD3DX12_CPU_DESCRIPTOR_HANDLE;

class ScreenSizeResources
{
public:
   ScreenSizeResources(
      const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice,
      const Microsoft::WRL::ComPtr<IDXGIFactory6>& pDXGIFactory,
      const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& pCommandQueue,
      const HWND hWnd,
      const UINT64 fenceValue,
      const unsigned int backBufferCount,
      const int width,
      const int height,
      const bool bAllowTearing,
      const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
      const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT
      );
   ~ScreenSizeResources();

   const UINT64 GetFenceValue();
   void SetFenceValue(const UINT64 value);

   void Prepare(
      ID3D12GraphicsCommandList*& pCommandList,
      D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_PRESENT,
      D3D12_RESOURCE_STATES afterState = D3D12_RESOURCE_STATE_RENDER_TARGET
      );
   void Clear();
   const bool Present(
      HRESULT& hr,
      const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& pCommandQueue,
      D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_RENDER_TARGET
      );
   void UpdateBackBufferIndex();
   const int GetBackBufferIndex() const { return m_backBufferIndex; }

   const int GetWidth() const { return m_width; }
   const int GetHeight() const { return m_height; }

   void WaitForGpu(const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& pCommandQueue) noexcept;
   void MoveToNextFrame(const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& pCommandQueue);

private:
   CD3DX12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const;
   CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

private:
   static const size_t MAX_BACK_BUFFER_COUNT = 3;

   int m_width;
   int m_height;
   bool m_bAllowTearing;
   unsigned int m_backBufferCount;
   unsigned int m_backBufferIndex;

   Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pCommandList;
   Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_pCommandAllocators[MAX_BACK_BUFFER_COUNT];
   UINT64 m_fenceValues[MAX_BACK_BUFFER_COUNT];
   Microsoft::WRL::ComPtr<ID3D12Fence> m_pFence;
   Microsoft::WRL::Wrappers::Event m_fenceEvent;

   Microsoft::WRL::ComPtr<IDXGISwapChain3> m_pSwapChain;
   Microsoft::WRL::ComPtr<ID3D12Resource> m_pRenderTargets[MAX_BACK_BUFFER_COUNT];
   Microsoft::WRL::ComPtr<ID3D12Resource> m_pDepthStencil;

   Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
   Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap;
   UINT m_rtvDescriptorSize;
   D3D12_VIEWPORT m_screenViewport;
   D3D12_RECT m_scissorRect;

};

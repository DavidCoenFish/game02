#pragma once

//enum class DeviceResourcesOptions
//{
//   None = 0,
//   AllowTearing = 0x1,
//   EnableHDR = 0x2
//};

class ScreenSizeResources;

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

   void Prepare();
   void Clear();
   const bool Present();

private:
   void GetAdapter(IDXGIAdapter1** ppAdapter, const D3D_FEATURE_LEVEL d3dFeatureLevel);
   void CreateWindowSizeDependentResources(
      const HWND hWnd
      );
   void MoveToNextFrame();
   //void UpdateColorSpace();

private:
   unsigned int m_backBufferCount;

   unsigned int m_options;
   Microsoft::WRL::ComPtr<IDXGIFactory6> m_pDXGIFactory;
   DWORD m_dxgiFactoryFlags;
   Microsoft::WRL::ComPtr<ID3D12Device> m_pDevice;
   Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_pCommandQueue;

   Microsoft::WRL::ComPtr<ID3D12Fence> m_pFence;
   Microsoft::WRL::Wrappers::Event m_fenceEvent;

   std::unique_ptr< ScreenSizeResources > m_pScreenSizeResources;

};

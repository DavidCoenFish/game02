#pragma once

//enum class DeviceResourcesOptions
//{
//   None = 0,
//   AllowTearing = 0x1,
//   EnableHDR = 0x2
//};

class DeviceResources
{
public:
   static const unsigned int c_AllowTearing    = 0x1;
   static const unsigned int c_EnableHDR       = 0x2;

   DeviceResources(
      const HWND hWnd,
      const D3D_FEATURE_LEVEL d3dFeatureLevel,
      const unsigned int options
      );
   ~DeviceResources();

private:
   void GetAdapter(IDXGIAdapter1** ppAdapter, const D3D_FEATURE_LEVEL d3dFeatureLevel);
   void UpdateColorSpace();

private:
   unsigned int m_options;
   Microsoft::WRL::ComPtr<IDXGIFactory6> m_pDXGIFactory;
   Microsoft::WRL::ComPtr<ID3D12Device> m_pD3D12Device;

};

#include "CommonPCH.h"

#include "Common/DrawSystem/DeviceResources.h"
#include "Common/DrawSystem/d3dx12.h"
#include "Common/Log/Log.h"
#include "Common/Util/Utf8.h"

DeviceResources::DeviceResources(
   const HWND hWnd,
   const D3D_FEATURE_LEVEL d3dFeatureLevel,
   const unsigned int options
   )
   : m_options(options)
{
   hWnd;
   DWORD dxgiFactoryFlags = 0;
#if defined(_DEBUG)
   // Enable the debug layer (requires the Graphics Tools "optional feature").
   //
   // NOTE: Enabling the debug layer after device creation will invalidate the active device.
   {
      Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
      if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
      {
         debugController->EnableDebugLayer();
      }
      else
      {
         LOG_MESSAGE_WARNING("WARNING: Direct3D Debug Device is not available\n");
      }

      Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
      if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
      {
         dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

         dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
         dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

         DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
         {
               80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
         };
         DXGI_INFO_QUEUE_FILTER filter = {};
         filter.DenyList.NumIDs = _countof(hide);
         filter.DenyList.pIDList = hide;
         dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
      }
   }
#endif

   DX::ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(m_pDXGIFactory.ReleaseAndGetAddressOf())));

   // Determines whether tearing support is available for fullscreen borderless windows.
   if (m_options & c_AllowTearing)
   {
      BOOL allowTearing = FALSE;
      HRESULT hr = m_pDXGIFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
      if (FAILED(hr) || !allowTearing)
      {
         m_options &= ~c_AllowTearing;

         LOG_MESSAGE_WARNING("WARNING: Variable refresh rate displays not supported");
      }
   }

   Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
   GetAdapter(adapter.GetAddressOf(), d3dFeatureLevel);

   // Create the DX12 API device object.
   HRESULT hr = D3D12CreateDevice(
      adapter.Get(),
      d3dFeatureLevel,
      IID_PPV_ARGS(m_pD3D12Device.ReleaseAndGetAddressOf())
      );
   DX::ThrowIfFailed(hr);

   m_pD3D12Device->SetName(L"DeviceResources");

#ifndef NDEBUG
   // Configure debug device (if active).
   Microsoft::WRL::ComPtr<ID3D12InfoQueue> d3dInfoQueue;
   if (SUCCEEDED(m_pD3D12Device.As(&d3dInfoQueue)))
   {
#ifdef _DEBUG
      d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
      d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif
      D3D12_MESSAGE_ID hide[] =
      {
         D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
         D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
         D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE
      };
      D3D12_INFO_QUEUE_FILTER filter = {};
      filter.DenyList.NumIDs = _countof(hide);
      filter.DenyList.pIDList = hide;
      d3dInfoQueue->AddStorageFilterEntries(&filter);
   }
#endif
}

DeviceResources::~DeviceResources()
{
   m_pD3D12Device.Reset();
   m_pDXGIFactory.Reset();
}

void DeviceResources::GetAdapter(IDXGIAdapter1** ppAdapter, const D3D_FEATURE_LEVEL d3dFeatureLevel)
{
   *ppAdapter = nullptr;

   Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
   for (UINT adapterIndex = 0;
      SUCCEEDED(m_pDXGIFactory->EnumAdapterByGpuPreference(
         adapterIndex,
         DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
         IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf())));
      adapterIndex++)
   {
      DXGI_ADAPTER_DESC1 desc;
      DX::ThrowIfFailed(adapter->GetDesc1(&desc));

      if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
      {
         // Don't select the Basic Render Driver adapter.
         continue;
      }

      // Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
      if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), d3dFeatureLevel, _uuidof(ID3D12Device), nullptr)))
      {
      #ifdef _DEBUG
         LOG_MESSAGE("Direct3D Adapter (%u): VID:%04X, PID:%04X - %s", adapterIndex, desc.VendorId, desc.DeviceId, Utf8::Utf16ToUtf8(desc.Description).c_str());
      #endif
         break;
      }
   }

#if !defined(NDEBUG)
   if (!adapter)
   {
      // Try WARP12 instead
      if (FAILED(m_pDXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf()))))
      {
         throw std::exception("WARP12 not available. Enable the 'Graphics Tools' optional feature");
      }

      LOG_MESSAGE("Direct3D Adapter - WARP12");
   }
#endif

   if (!adapter)
   {
      throw std::exception("No Direct3D 12 device found");
   }

   *ppAdapter = adapter.Detach();
}


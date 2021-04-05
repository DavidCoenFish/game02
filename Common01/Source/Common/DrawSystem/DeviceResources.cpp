#include "CommonPCH.h"

#include "Common/DrawSystem/DeviceResources.h"
#include "Common/DrawSystem/ScreenSizeResources.h"
#include "Common/DrawSystem/d3dx12.h"
#include "Common/Log/Log.h"
#include "Common/Util/Utf8.h"

DeviceResources::DeviceResources(
   const HWND hWnd,
   const unsigned int backBufferCount,
   const D3D_FEATURE_LEVEL d3dFeatureLevel,
   const unsigned int options
   )
   : m_options(options)
   , m_backBufferCount(backBufferCount)
   , m_dxgiFactoryFlags(0)
{
   hWnd;

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
         m_dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

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

   DX::ThrowIfFailed(CreateDXGIFactory2(m_dxgiFactoryFlags, IID_PPV_ARGS(m_pDXGIFactory.ReleaseAndGetAddressOf())));

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
      IID_PPV_ARGS(m_pDevice.ReleaseAndGetAddressOf())
      );
   DX::ThrowIfFailed(hr);

   m_pDevice->SetName(L"DeviceResources");

#ifndef NDEBUG
   // Configure debug device (if active).
   Microsoft::WRL::ComPtr<ID3D12InfoQueue> d3dInfoQueue;
   if (SUCCEEDED(m_pDevice.As(&d3dInfoQueue)))
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

   // Create the command queue.
   D3D12_COMMAND_QUEUE_DESC queueDesc = {};
   queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
   queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

   DX::ThrowIfFailed(m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_pCommandQueue.ReleaseAndGetAddressOf())));

   m_pCommandQueue->SetName(L"DeviceResources");

   CreateWindowSizeDependentResources(hWnd);

   if (nullptr != m_pScreenSizeResources)
   {
      // Create a fence for tracking GPU execution progress.
      const auto fenceValue = m_pScreenSizeResources->GetFenceValue();
      DX::ThrowIfFailed(m_pDevice->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf())));
      m_pScreenSizeResources->SetFenceValue(fenceValue + 1);
      m_pFence->SetName(L"Fence");
   }

   m_fenceEvent.Attach(CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
   if (!m_fenceEvent.IsValid())
   {
      throw std::exception("CreateEvent");
   }
}

DeviceResources::~DeviceResources()
{
   WaitForGpu();

   for (UINT n = 0; n < m_backBufferCount; n++)
   {
      //m_commandAllocators[n].Reset();
      //m_renderTargets[n].Reset();
   }

   //m_depthStencil.Reset();
   m_pCommandQueue.Reset();
   //m_commandList.Reset();
   m_pFence.Reset();
   //m_rtvDescriptorHeap.Reset();
   //m_dsvDescriptorHeap.Reset();

   m_pScreenSizeResources.reset();
     //m_swapChain.Reset();

   m_pDevice.Reset();
   m_pDXGIFactory.Reset();

#ifdef _DEBUG
    {
        Microsoft::WRL::ComPtr<IDXGIDebug1> dxgiDebug;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
        {
            dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
        }
    }
#endif
}

void DeviceResources::WaitForGpu() noexcept
{
   if (m_pCommandQueue && m_pFence && m_fenceEvent.IsValid() && m_pScreenSizeResources)
   {
      // Schedule a Signal command in the GPU queue.
      UINT64 fenceValue = m_pScreenSizeResources->GetFenceValue();
      if (SUCCEEDED(m_pCommandQueue->Signal(m_pFence.Get(), fenceValue)))
      {
         // Wait until the Signal has been processed.
         if (SUCCEEDED(m_pFence->SetEventOnCompletion(fenceValue, m_fenceEvent.Get())))
         {
            WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);

            // Increment the fence value for the current frame.
            m_pScreenSizeResources->SetFenceValue(fenceValue + 1);
         }
      }
   }
   return;
}

void DeviceResources::OnResize(const HWND hWnd)
{
   CreateWindowSizeDependentResources(hWnd);
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

void DeviceResources::CreateWindowSizeDependentResources(
   const HWND hWnd
   )
{
   RECT rc;
   GetClientRect(hWnd, &rc);
   const int width = rc.right - rc.left;
   const int height = rc.bottom - rc.top;

   //if we don't need to resize, then don't
   if ((nullptr != m_pScreenSizeResources) &&
      (width == m_pScreenSizeResources->GetWidth()) &&
      (height == m_pScreenSizeResources->GetHeight()))
   {
      return;
   }

   WaitForGpu();

   UINT64 fenceValue = 0;
   if (nullptr != m_pScreenSizeResources)
   {
      fenceValue = m_pScreenSizeResources->GetFenceValue();
   }
   m_pScreenSizeResources.reset();

   const unsigned int swapFlag = (m_options & c_AllowTearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;
   m_pScreenSizeResources = std::make_unique<ScreenSizeResources>(
      m_pDevice,
      m_pDXGIFactory,
      m_pCommandQueue,
      hWnd,
      fenceValue,
      m_backBufferCount,
      width,
      height,
      swapFlag
      );
}

void DeviceResources::MoveToNextFrame()
{
   // Schedule a Signal command in the queue.
   const UINT64 currentFenceValue = m_pScreenSizeResources->GetFenceValue();
   DX::ThrowIfFailed(m_pCommandQueue->Signal(m_pFence.Get(), currentFenceValue));

   m_pScreenSizeResources->UpdateBackBufferIndex();
   const UINT64 nextBackBufferFenceValue = m_pScreenSizeResources->GetFenceValue();
   // If the next frame is not ready to be rendered yet, wait until it is ready.
   if (m_pFence->GetCompletedValue() < nextBackBufferFenceValue)
   {
      DX::ThrowIfFailed(m_pFence->SetEventOnCompletion(nextBackBufferFenceValue, m_fenceEvent.Get()));
      WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);
   }

   // Set the fence value for the next frame.
   m_pScreenSizeResources->SetFenceValue(currentFenceValue + 1);
}

void DeviceResources::Prepare()
{
   if (nullptr == m_pScreenSizeResources)
   {
      return;
   }
   m_pScreenSizeResources->Prepare();
}

void DeviceResources::Clear()
{
   if (nullptr == m_pScreenSizeResources)
   {
      return;
   }
   m_pScreenSizeResources->Clear();
}

const bool DeviceResources::Present()
{
   if (nullptr == m_pScreenSizeResources)
   {
      return false;
   }
   HRESULT hr = SEVERITY_SUCCESS;
   if (false == m_pScreenSizeResources->Present(hr, m_pCommandQueue))
   {
#ifdef _DEBUG
        char buff[64] = {};
        sprintf_s(buff, "Device Lost on Present: Reason code 0x%08X\n",
            static_cast<unsigned int>((hr == DXGI_ERROR_DEVICE_REMOVED) ? m_pDevice->GetDeviceRemovedReason() : hr));
        OutputDebugStringA(buff);
#endif

        return false;
   }

   MoveToNextFrame();

   if (!m_pDXGIFactory->IsCurrent())
   {
      // Output information is cached on the DXGI Factory. If it is stale we need to create a new factory.
      DX::ThrowIfFailed(CreateDXGIFactory2(m_dxgiFactoryFlags, IID_PPV_ARGS(m_pDXGIFactory.ReleaseAndGetAddressOf())));
   }

   return true;
}


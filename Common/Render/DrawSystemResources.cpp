#include "CommonPCH.h"

#include "Common/Render/DrawSystemResources.h"
#include "Common/Application/Log.h"
#include "Common/Render/DrawSystem.h"
#include "Common/Render/iResource.h"
#include "Common/Render/CustomCommandList.h"
#include "Common/Render/HeapWrapperItem.h"
#include "Common/Render/HeapWrapper.h"
#include "Common/Render/RenderTargetBackbuffer.h"
#include "Common/DirectXTK12/GraphicsMemory.h"

DrawSystemResources::DrawSystemResources(
   DrawSystem* const pDrawSystem,
   unsigned int& flags, 
   D3D_FEATURE_LEVEL minFeatureLevel,
   const int backBufferCount,
   const RenderTargetFormatData& targetFormatData,
   const RenderTargetDepthData& targetDepthData
   )
   : m_d3dFeatureLevel(D3D_FEATURE_LEVEL_11_0)
   , m_dxgiFactoryFlags(0)
   , m_backBufferCount(backBufferCount)
   , m_backBufferIndex(0)
{
   for (int index = 0; index < MAX_BACK_BUFFER_COUNT; ++index)
   {
      m_fenceValues[index] = 0;
   }

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

    DX::ThrowIfFailed(CreateDXGIFactory2(m_dxgiFactoryFlags, IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));

    // Determines whether tearing support is available for fullscreen borderless windows.
    if (flags & DrawSystem::c_AllowTearing)
    {
        BOOL allowTearing = FALSE;
        HRESULT hr = m_dxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
        if (FAILED(hr) || !allowTearing)
        {
            flags &= ~DrawSystem::c_AllowTearing;
#ifdef _DEBUG
            LOG_MESSAGE_WARNING("WARNING: Variable refresh rate displays not supported");
#endif
        }
    }

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    GetAdapter(adapter.GetAddressOf(), minFeatureLevel);

    // Create the DX12 API device object.
    DX::ThrowIfFailed(D3D12CreateDevice(
        adapter.Get(),
        minFeatureLevel,
        IID_PPV_ARGS(m_d3dDevice.ReleaseAndGetAddressOf())
        ));

    m_d3dDevice->SetName(L"Device");

#ifndef NDEBUG
    // Configure debug device (if active).
    Microsoft::WRL::ComPtr<ID3D12InfoQueue> d3dInfoQueue;
    if (SUCCEEDED(m_d3dDevice.As(&d3dInfoQueue)))
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

    // Determine maximum supported feature level for this device
    static const D3D_FEATURE_LEVEL s_featureLevels[] =
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels =
    {
        _countof(s_featureLevels), s_featureLevels, D3D_FEATURE_LEVEL_11_0
    };

    HRESULT hr = m_d3dDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
    if (SUCCEEDED(hr))
    {
        m_d3dFeatureLevel = featLevels.MaxSupportedFeatureLevel;
    }
    else
    {
        m_d3dFeatureLevel = minFeatureLevel;
    }

    // Create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    DX::ThrowIfFailed(m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_commandQueue.ReleaseAndGetAddressOf())));

    m_commandQueue->SetName(L"Command Queue");

    // Create a command allocator for each back buffer that will be rendered to.
    for (int n = 0; n < backBufferCount; n++)
    {
        DX::ThrowIfFailed(m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_commandAllocators[n].ReleaseAndGetAddressOf())));

        wchar_t name[25] = {};
        swprintf_s(name, L"Render target %u", n);
        m_commandAllocators[n]->SetName(name);
    }

    // Create a command list for recording graphics commands.
    DX::ThrowIfFailed(m_d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[m_backBufferIndex].Get(), nullptr, IID_PPV_ARGS(m_commandList.ReleaseAndGetAddressOf())));
    DX::ThrowIfFailed(m_commandList->Close());

    m_commandList->SetName(L"Command List DrawSystemResources");

    // Create a fence for tracking GPU execution progress.
    DX::ThrowIfFailed(m_d3dDevice->CreateFence(m_fenceValues[m_backBufferIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf())));
    m_fenceValues[m_backBufferIndex]++;

    m_fence->SetName(L"Fence");

    m_fenceEvent.Attach(CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
    if (!m_fenceEvent.IsValid())
    {
        throw std::exception("CreateEvent");
    }

   for (int index = 0; index < m_backBufferCount; ++index)
   {
      m_renderTargetBackBufferArray[index] = std::make_shared< RenderTargetBackBuffer >(
         pDrawSystem,
         index,
         targetFormatData,
         targetDepthData
         );
   }

   m_pGraphicsMemory = std::make_unique<DirectX::GraphicsMemory>(m_d3dDevice.Get());

   return;
}

DrawSystemResources::~DrawSystemResources()
{
   for (int n = 0; n < m_backBufferCount; n++)
   {
      m_commandAllocators[n].Reset();
      m_renderTargetBackBufferArray[n].reset();
   }
   m_commandQueue.Reset();
   m_commandList.Reset();
   m_fence.Reset();
   m_swapChain.Reset();
   m_pGraphicsMemory.reset();
   m_d3dDevice.Reset();
   m_dxgiFactory.Reset();

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

void DrawSystemResources::GetAdapter(IDXGIAdapter1** ppAdapter, D3D_FEATURE_LEVEL minFeatureLevel)
{
    *ppAdapter = nullptr;

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    for (UINT adapterIndex = 0;
        SUCCEEDED(m_dxgiFactory->EnumAdapterByGpuPreference(
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
        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), minFeatureLevel, _uuidof(ID3D12Device), nullptr)))
        {
            LOG_MESSAGE_RENDER("Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", adapterIndex, desc.VendorId, desc.DeviceId, desc.Description);
            break;
        }
    }

#if !defined(NDEBUG)
    if (!adapter)
    {
        // Try WARP12 instead
        if (FAILED(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf()))))
        {
            throw std::exception("WARP12 not available. Enable the 'Graphics Tools' optional feature");
        }

        LOG_MESSAGE_RENDER("Direct3D Adapter - WARP12\n");
    }
#endif

    if (!adapter)
    {
        throw std::exception("No Direct3D 12 device found");
    }

    *ppAdapter = adapter.Detach();
}

HRESULT DrawSystemResources::CreateWindowSizeDependentResources(
   const int backBufferCount,
   const int backBufferWidth,
   const int backBufferHeight,
   const DXGI_FORMAT backBufferFormat,
   const int flags,
   const HWND hWnd
   )
{
   LOG_MESSAGE_RENDER("DrawSystemResources::CreateWindowSizeDependentResources %d %d\n", backBufferWidth, backBufferHeight);

   WaitForGpu();

   for (int index = 0; index < m_backBufferCount; ++index)
   {
      iResource* const pBackBuffer = m_renderTargetBackBufferArray[index].get();
      if (pBackBuffer)
      {
         pBackBuffer->OnDeviceLost();
      }
   }

   // Release resources that are tied to the swap chain and update fence values.
   for (int index = 0; index < m_backBufferCount; index++)
   {
      m_fenceValues[index] = m_fenceValues[m_backBufferIndex];
   }

    if (m_swapChain)
    {
        // If the swap chain already exists, resize it.
        HRESULT hr = m_swapChain->ResizeBuffers(
            backBufferCount,
            backBufferWidth,
            backBufferHeight,
            backBufferFormat,
            (flags & DrawSystem::c_AllowTearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u
            );

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
           return hr;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = backBufferFormat;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = backBufferCount;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        swapChainDesc.Flags = (flags & DrawSystem::c_AllowTearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a swap chain for the window.
        Microsoft::WRL::ComPtr<IDXGISwapChain1> localSwapChain;
        DX::ThrowIfFailed(m_dxgiFactory->CreateSwapChainForHwnd(
            m_commandQueue.Get(),
            hWnd,
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr,
            localSwapChain.GetAddressOf()
            ));

        DX::ThrowIfFailed(localSwapChain.As(&m_swapChain));

        // This class does not support exclusive full-screen mode and prevents DXGI from responding to the ALT+ENTER shortcut
        DX::ThrowIfFailed(m_dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
    }

    m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

    // we can not run this before the HeapWrapper's get OnDeviceRestored...
   //for (int index = 0; index < m_backBufferCount; ++index)
   //{
   //   iResource* const pBackBuffer = m_renderTargetBackBufferArray[index].get();
   //   if (pBackBuffer)
   //   {
   //      pBackBuffer->OnDeviceRestored( m_d3dDevice.Get() );
   //   }
   //}

   return S_OK;
}

void DrawSystemResources::OnWindowSizeChanged(
   CustomCommandList* const pCommandList
   )
{
   for (int index = 0; index < m_backBufferCount; ++index)
   {
      iResource* const pBackBuffer = m_renderTargetBackBufferArray[index].get();
      if (pBackBuffer)
      {
         pBackBuffer->OnDeviceRestored( pCommandList, m_d3dDevice.Get() );
      }
   }
   return;
}

DirectX::GraphicsResource DrawSystemResources::AllocateUpload(const std::size_t size, void* const pDataOrNullptr, size_t alignment)
{
   const size_t alignedSize = (size + alignment - 1) & ~(alignment - 1);
   auto graphicsResource = m_pGraphicsMemory->Allocate(alignedSize, alignment);
   if (pDataOrNullptr)
   {
      memcpy(graphicsResource.Memory(), pDataOrNullptr, size);
   }
   return graphicsResource;
}

IDXGISwapChain3* DrawSystemResources::GetSwapChain() const
{
   return m_swapChain.Get();
}

void DrawSystemResources::WaitForGpu() noexcept
{
    if (m_commandQueue && m_fence && m_fenceEvent.IsValid())
    {
        // Schedule a Signal command in the GPU queue.
        UINT64 fenceValue = m_fenceValues[m_backBufferIndex];
        if (SUCCEEDED(m_commandQueue->Signal(m_fence.Get(), fenceValue)))
        {
            // Wait until the Signal has been processed.
            if (SUCCEEDED(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent.Get())))
            {
                WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);

                // Increment the fence value for the current frame.
                m_fenceValues[m_backBufferIndex]++;
            }
        }
    }
}

std::shared_ptr< CustomCommandList > DrawSystemResources::MakeCommandList()
{
   // Reset command list and allocator.
   DX::ThrowIfFailed(m_commandAllocators[m_backBufferIndex]->Reset());
   DX::ThrowIfFailed(m_commandList->Reset(m_commandAllocators[m_backBufferIndex].Get(), nullptr));

   auto pResult = std::make_shared< CustomCommandList >(
      m_d3dDevice.Get(),
      m_commandQueue.Get(),
      m_commandList.Get(),
      nullptr,
      m_backBufferIndex
      );
   return pResult;
}


// Present the contents of the swap chain to the screen.
HRESULT DrawSystemResources::Present(const int flags)
{
    HRESULT hr;
    if (flags & DrawSystem::c_AllowTearing)
    {
        // Recommended to always use tearing if supported when using a sync interval of 0.
        // Note this will fail if in true 'fullscreen' mode.
        hr = m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
    }
    else
    {
        // The first argument instructs DXGI to block until VSync, putting the application
        // to sleep until the next VSync. This ensures we don't waste any cycles rendering
        // frames that will never be displayed to the screen.
        hr = m_swapChain->Present(1, 0);
    }

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        LOG_MESSAGE_WARNING("Device Lost on Present: Reason code 0x%08X\n",(hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr );
        //pParent->HandleDeviceLost();
        return hr;
    }
    else
    {
        DX::ThrowIfFailed(hr);

        MoveToNextFrame();

        if (!m_dxgiFactory->IsCurrent())
        {
            // Output information is cached on the DXGI Factory. If it is stale we need to create a new factory.
            DX::ThrowIfFailed(CreateDXGIFactory2(m_dxgiFactoryFlags, IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));
        }
    }

    if (m_pGraphicsMemory)
    {
       m_pGraphicsMemory->Commit(m_commandQueue.Get());
    }

    return hr;
}

// Prepare to render the next frame.
void DrawSystemResources::MoveToNextFrame()
{
    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = m_fenceValues[m_backBufferIndex];
    DX::ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

    // Update the back buffer index.
    m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (m_fence->GetCompletedValue() < m_fenceValues[m_backBufferIndex])
    {
        DX::ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_backBufferIndex], m_fenceEvent.Get()));
        WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    m_fenceValues[m_backBufferIndex] = currentFenceValue + 1;
}

HRESULT DrawSystemResources::GetDeviceRemovedReason()
{
   if (m_d3dDevice)
   {
      m_d3dDevice->GetDeviceRemovedReason();
   }
   return E_FAIL;
}

void DrawSystemResources::DeviceRestored(
   iResource* const pResource,
   CustomCommandList* const pCommandList
   )
{
   if (m_d3dDevice && pResource)
   {
      pResource->OnDeviceRestored( 
         pCommandList,
         m_d3dDevice.Get()
         );
   }
}

ID3D12Device* const DrawSystemResources::GetD3dDevice() // m_d3dDevice;
{
   return m_d3dDevice.Get();
}

iRenderTarget* const DrawSystemResources::GetCurrentRenderTargetBackbuffer()
{
   return m_renderTargetBackBufferArray[m_backBufferIndex].get();
}

//std::shared_ptr<HeapWrapperItem> DrawSystemResources::MakeHeapWrapperItem( 
//   const std::shared_ptr< HeapWrapper >& heapWrapper,
//   const int length
//   )
//{
//   return HeapWrapperItem::Factory( 
//      m_d3dDevice.Get(),
//      heapWrapper, 
//      length 
//      );
//}


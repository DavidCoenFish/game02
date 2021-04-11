#include "CommonPCH.h"

#include "Common/DrawSystem/ScreenSizeResources.h"
#include "Common/DrawSystem/d3dx12.h"

static const DXGI_FORMAT NoSRGB(DXGI_FORMAT fmt) noexcept
{
   switch (fmt)
   {
   case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:   return DXGI_FORMAT_R8G8B8A8_UNORM;
   case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8A8_UNORM;
   case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8X8_UNORM;
   default:                                return fmt;
   }
}


ScreenSizeResources::ScreenSizeResources(
   const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice,
   const Microsoft::WRL::ComPtr<IDXGIFactory6>& pDXGIFactory,
   const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& pCommandQueue,
   const HWND hWnd,
   const UINT64 fenceValue,
   const unsigned int backBufferCount,
   const int width,
   const int height,
   const bool bAllowTearing,
   const DXGI_FORMAT _backBufferFormat,
   const DXGI_FORMAT depthBufferFormat
   )
   : m_backBufferCount(backBufferCount)
   , m_backBufferIndex(0)
   , m_width(width)
   , m_height(height)
   , m_bAllowTearing(bAllowTearing)
   , m_rtvDescriptorSize(0)
   , m_screenViewport{}
   , m_scissorRect{}
{
   for (unsigned int index = 0; index < MAX_BACK_BUFFER_COUNT; ++index)
   {
      m_fenceValues[index] = fenceValue;
   }

   // Create descriptor heaps for render target views and depth stencil views.
   D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc = {};
   rtvDescriptorHeapDesc.NumDescriptors = m_backBufferCount;
   rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

   DX::ThrowIfFailed(pDevice->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(m_rtvDescriptorHeap.ReleaseAndGetAddressOf())));

   m_rtvDescriptorHeap->SetName(L"rtvDescriptorHeap");

   m_rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

   if (depthBufferFormat != DXGI_FORMAT_UNKNOWN)
   {
      D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc = {};
      dsvDescriptorHeapDesc.NumDescriptors = 1;
      dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

      DX::ThrowIfFailed(pDevice->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(m_dsvDescriptorHeap.ReleaseAndGetAddressOf())));

      m_dsvDescriptorHeap->SetName(L"dsvDescriptorHeap");
   }

   // Create a command allocator for each back buffer that will be rendered to.
   for (UINT n = 0; n < m_backBufferCount; n++)
   {
      DX::ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_pCommandAllocators[n].ReleaseAndGetAddressOf())));

      wchar_t name[25] = {};
      swprintf_s(name, L"Command Allocators %u", n);
      m_pCommandAllocators[n]->SetName(name);
   }

   // Create a command list for recording graphics commands.
   DX::ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocators[0].Get(), nullptr, IID_PPV_ARGS(m_pCommandList.ReleaseAndGetAddressOf())));
   DX::ThrowIfFailed(m_pCommandList->Close());
   {
      static int count = -1;
      count += 1;
      wchar_t name[25] = {};
      swprintf_s(name, L"CommandList:%d", count);
      m_pCommandList->SetName(name);
   }

   const UINT backBufferWidth = std::max<UINT>(width, 1u);
   const UINT backBufferHeight = std::max<UINT>(height, 1u);
   const DXGI_FORMAT backBufferFormat = NoSRGB(_backBufferFormat);

   {
      // Create a descriptor for the swap chain.
      DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
      swapChainDesc.Width = backBufferWidth;
      swapChainDesc.Height = backBufferHeight;
      swapChainDesc.Format = backBufferFormat;
      swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      swapChainDesc.BufferCount = m_backBufferCount;
      swapChainDesc.SampleDesc.Count = 1;
      swapChainDesc.SampleDesc.Quality = 0;
      swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
      swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
      swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
      swapChainDesc.Flags = m_bAllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;

      DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
      fsSwapChainDesc.Windowed = TRUE;

      // Create a swap chain for the window.
      Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
      DX::ThrowIfFailed(pDXGIFactory->CreateSwapChainForHwnd(
         pCommandQueue.Get(),
         hWnd,
         &swapChainDesc,
         &fsSwapChainDesc,
         nullptr,
         swapChain.GetAddressOf()
         ));

      DX::ThrowIfFailed(swapChain.As(&m_pSwapChain));

      // This class does not support exclusive full-screen mode and prevents DXGI from responding to the ALT+ENTER shortcut
      DX::ThrowIfFailed(pDXGIFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
   }

   // Handle color space settings for HDR
   //UpdateColorSpace();

   // Obtain the back buffers for this window which will be the final render targets
   // and create render target views for each of them.
   for (UINT n = 0; n < m_backBufferCount; n++)
   {
      DX::ThrowIfFailed(m_pSwapChain->GetBuffer(n, IID_PPV_ARGS(m_pRenderTargets[n].GetAddressOf())));

      wchar_t name[25] = {};
      swprintf_s(name, L"Render target %u", n);
      m_pRenderTargets[n]->SetName(name);

      D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
      rtvDesc.Format = backBufferFormat;
      rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

      CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(
         m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
         static_cast<INT>(n), m_rtvDescriptorSize);
     pDevice->CreateRenderTargetView(m_pRenderTargets[n].Get(), &rtvDesc, rtvDescriptor);
   }

   // Reset the index to the current back buffer.
   m_backBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

   if (depthBufferFormat != DXGI_FORMAT_UNKNOWN)
   {
      // Allocate a 2-D surface as the depth/stencil buffer and create a depth/stencil view
      // on this surface.
      CD3DX12_HEAP_PROPERTIES depthHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

      D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
         depthBufferFormat,
         backBufferWidth,
         backBufferHeight,
         1, // This depth stencil view has only one texture.
         1  // Use a single mipmap level.
         );
      depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

      D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
      depthOptimizedClearValue.Format = depthBufferFormat;
      depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
      depthOptimizedClearValue.DepthStencil.Stencil = 0;

      DX::ThrowIfFailed(pDevice->CreateCommittedResource(
         &depthHeapProperties,
         D3D12_HEAP_FLAG_NONE,
         &depthStencilDesc,
         D3D12_RESOURCE_STATE_DEPTH_WRITE,
         &depthOptimizedClearValue,
         IID_PPV_ARGS(m_pDepthStencil.ReleaseAndGetAddressOf())
         ));

      m_pDepthStencil->SetName(L"Depth stencil");

      D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
      dsvDesc.Format = depthBufferFormat;
      dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

      pDevice->CreateDepthStencilView(m_pDepthStencil.Get(), &dsvDesc, m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
   }

   {
      // Create a fence for tracking GPU execution progress.
      const auto fenceValue = GetFenceValue();
      DX::ThrowIfFailed(pDevice->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf())));
      SetFenceValue(fenceValue + 1);
      m_pFence->SetName(L"Fence");
   }

   m_fenceEvent.Attach(CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
   if (!m_fenceEvent.IsValid())
   {
      throw std::exception("CreateEvent");
   }

   // Set the 3D rendering viewport and scissor rectangle to target the entire window.
   m_screenViewport.TopLeftX = m_screenViewport.TopLeftY = 0.f;
   m_screenViewport.Width = static_cast<float>(backBufferWidth);
   m_screenViewport.Height = static_cast<float>(backBufferHeight);
   m_screenViewport.MinDepth = D3D12_MIN_DEPTH;
   m_screenViewport.MaxDepth = D3D12_MAX_DEPTH;

   m_scissorRect.left = m_scissorRect.top = 0;
   m_scissorRect.right = static_cast<LONG>(backBufferWidth);
   m_scissorRect.bottom = static_cast<LONG>(backBufferHeight);
}

ScreenSizeResources::~ScreenSizeResources()
{
}

const UINT64 ScreenSizeResources::GetFenceValue()
{
   return m_fenceValues[m_backBufferIndex];
}

void ScreenSizeResources::SetFenceValue(const UINT64 value)
{
   m_fenceValues[m_backBufferIndex] = value;
   return;
}

void ScreenSizeResources::Prepare(
   ID3D12GraphicsCommandList*& pCommandList,
   D3D12_RESOURCE_STATES beforeState,
   D3D12_RESOURCE_STATES afterState
   )
{
    // Reset command list and allocator.
    DX::ThrowIfFailed(m_pCommandAllocators[m_backBufferIndex]->Reset());
    DX::ThrowIfFailed(m_pCommandList->Reset(m_pCommandAllocators[m_backBufferIndex].Get(), nullptr));
    pCommandList = m_pCommandList.Get();

    if (beforeState != afterState)
    {
        // Transition the render target into the correct state to allow for drawing into it.
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargets[m_backBufferIndex].Get(),
            beforeState, afterState);
        m_pCommandList->ResourceBarrier(1, &barrier);
    }

    return;
}

void ScreenSizeResources::Clear()
{
   //PIXBeginEvent(m_pCommandList, PIX_COLOR_DEFAULT, L"Clear"); // See pch.h for info

   // Clear the views.
   auto rtvDescriptor = GetRenderTargetView();
   auto dsvDescriptor = GetDepthStencilView();

   m_pCommandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, &dsvDescriptor);
   m_pCommandList->ClearRenderTargetView(rtvDescriptor, DirectX::Colors::CornflowerBlue, 0, nullptr);
   m_pCommandList->ClearDepthStencilView(dsvDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

   // Set the viewport and scissor rect.
   m_pCommandList->RSSetViewports(1, &m_screenViewport);
   m_pCommandList->RSSetScissorRects(1, &m_scissorRect);

   //PIXEndEvent(commandList);
}

void ScreenSizeResources::WaitForGpu(const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& pCommandQueue) noexcept
{
   if (pCommandQueue && m_pFence && m_fenceEvent.IsValid())
   {
      // Schedule a Signal command in the GPU queue.
      UINT64 fenceValue = GetFenceValue();
      if (SUCCEEDED(pCommandQueue->Signal(m_pFence.Get(), fenceValue)))
      {
         // Wait until the Signal has been processed.
         if (SUCCEEDED(m_pFence->SetEventOnCompletion(fenceValue, m_fenceEvent.Get())))
         {
            WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);

            // Increment the fence value for the current frame.
            SetFenceValue(fenceValue + 1);
         }
      }
   }
   return;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE ScreenSizeResources::GetRenderTargetView() const
{
   return CD3DX12_CPU_DESCRIPTOR_HANDLE(
         m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
         static_cast<INT>(m_backBufferIndex), m_rtvDescriptorSize);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE ScreenSizeResources::GetDepthStencilView() const
{
   return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

const bool ScreenSizeResources::Present(
   HRESULT& hr,
   const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& pCommandQueue,
   D3D12_RESOURCE_STATES beforeState
   )
{
    if (beforeState != D3D12_RESOURCE_STATE_PRESENT)
    {
        // Transition the render target to the state that allows it to be presented to the display.
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargets[m_backBufferIndex].Get(), beforeState, D3D12_RESOURCE_STATE_PRESENT);
        m_pCommandList->ResourceBarrier(1, &barrier);
    }

    // Send the command list off to the GPU for processing.
    DX::ThrowIfFailed(m_pCommandList->Close());
    pCommandQueue->ExecuteCommandLists(1, CommandListCast(m_pCommandList.GetAddressOf()));

    if (m_bAllowTearing)
    {
        // Recommended to always use tearing if supported when using a sync interval of 0.
        // Note this will fail if in true 'fullscreen' mode.
        hr = m_pSwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
    }
    else
    {
        // The first argument instructs DXGI to block until VSync, putting the application
        // to sleep until the next VSync. This ensures we don't waste any cycles rendering
        // frames that will never be displayed to the screen.
        hr = m_pSwapChain->Present(1, 0);
    }

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        return false;
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
    return true;
}

void ScreenSizeResources::UpdateBackBufferIndex()
{
   // Update the back buffer index.
   m_backBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

void ScreenSizeResources::MoveToNextFrame(const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& pCommandQueue)
{
   // Schedule a Signal command in the queue.
   const UINT64 currentFenceValue = GetFenceValue();
   DX::ThrowIfFailed(pCommandQueue->Signal(m_pFence.Get(), currentFenceValue));

   UpdateBackBufferIndex();
   const UINT64 nextBackBufferFenceValue = GetFenceValue();
   // If the next frame is not ready to be rendered yet, wait until it is ready.
   if (m_pFence->GetCompletedValue() < nextBackBufferFenceValue)
   {
      DX::ThrowIfFailed(m_pFence->SetEventOnCompletion(nextBackBufferFenceValue, m_fenceEvent.Get()));
      WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);
   }

   // Set the fence value for the next frame.
   SetFenceValue(currentFenceValue + 1);
}

#include "CommonPCH.h"

#include "Common/Render/DrawSystemResourcesSize.h"
#include "Common/Render/DrawSystem.h"
#include "Common/Render/HeapWrapperItem.h"
#include "Common/Render/RenderTargetBackbuffer.h"

DrawSystemResourcesSize::DrawSystemResourcesSize(
   DrawSystem* const pDrawSystem,
   Microsoft::WRL::ComPtr<IDXGISwapChain3>& swapChain,
   const int backBufferCount,
   const int backBufferWidth,
   const int backBufferHeight,
   const DXGI_FORMAT backBufferFormat,
   const DXGI_FORMAT depthBufferFormat,
   const int flags,
   const HWND hWnd,
   const Microsoft::WRL::ComPtr<IDXGIFactory6>& dxgiFactory,
   const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& commandQueue,
   int& backBufferIndex,
   const Microsoft::WRL::ComPtr<ID3D12Device>& d3dDevice
   )
   : m_hr(E_FAIL)
{
    // If the swap chain already exists, resize it, otherwise create one.
    if (swapChain)
    {
        // If the swap chain already exists, resize it.
        m_hr = swapChain->ResizeBuffers(
            backBufferCount,
            backBufferWidth,
            backBufferHeight,
            backBufferFormat,
            (flags & DrawSystem::c_AllowTearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u
            );

        if (m_hr == DXGI_ERROR_DEVICE_REMOVED || m_hr == DXGI_ERROR_DEVICE_RESET)
        {
           return;
        }
        else
        {
            DX::ThrowIfFailed(m_hr);
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
        DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
            commandQueue.Get(),
            hWnd,
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr,
            localSwapChain.GetAddressOf()
            ));

        DX::ThrowIfFailed(localSwapChain.As(&swapChain));

        // This class does not support exclusive full-screen mode and prevents DXGI from responding to the ALT+ENTER shortcut
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
    }
    m_hr = S_OK;

    backBufferIndex = swapChain->GetCurrentBackBufferIndex();

    D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
    depthOptimizedClearValue.Format = depthBufferFormat;
    depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

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

        DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
            &depthHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &depthStencilDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(m_depthStencil.ReleaseAndGetAddressOf())
            ));

        m_depthStencil->SetName(L"Depth stencil");

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = depthBufferFormat;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

        m_pDepthStencilViewDescriptor = pDrawSystem->MakeHeapWrapperDepthStencilView();
        d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &dsvDesc, m_pDepthStencilViewDescriptor->GetCPUHandle());
    }

   for (int index = 0; index < backBufferCount; ++index)
   {
      m_pRenderTargetBackbuffer[index] = RenderTargetBackBuffer::Factory(
         pDrawSystem,
         d3dDevice,
         swapChain,
         index,
         backBufferFormat,
         depthBufferFormat,
         m_depthStencil,
         m_pDepthStencilViewDescriptor,
         depthOptimizedClearValue,
         backBufferWidth,
         backBufferHeight
         );
   }
}

DrawSystemResourcesSize::~DrawSystemResourcesSize()
{
   return;
}

iRenderTarget* const DrawSystemResourcesSize::GetRenderTarget( const int backBufferIndex )
{
   std::shared_ptr<RenderTargetBackBuffer>& pAuto = m_pRenderTargetBackbuffer[ backBufferIndex ];
   iRenderTarget* const pRenderTarget = pAuto.get();
   return pRenderTarget;
}

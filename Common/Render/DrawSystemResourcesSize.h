#pragma once

#include "Common/Render/RenderTargetBackbuffer.h"

class RenderTargetBackbuffer;
class HeapWrapperItem;
class iRenderTarget;
class DrawSystem;

class DrawSystemResourcesSize
{
public:
   DrawSystemResourcesSize(
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
      );
   ~DrawSystemResourcesSize();
   const HRESULT GetHResult( void ) const { return m_hr; }

   //iRenderTarget* const GetRenderTarget( const int backBufferIndex );

private:
   //static const size_t MAX_BACK_BUFFER_COUNT = 3;

   HRESULT m_hr;

   //std::shared_ptr<RenderTargetBackBuffer> m_pRenderTargetBackbuffer[MAX_BACK_BUFFER_COUNT];

   // the back bufer depth resource could be in the RenderTargetBackbuffer, but only need one?
   std::shared_ptr<HeapWrapperItem> m_pDepthStencilViewDescriptor;
   Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencil;

};

#pragma once

#include "Common/Render/iRenderTarget.h"
#include "Common/Render/RenderTargetDepthData.h"
#include "Common/Render/RenderTargetFormatData.h"

class DrawSystem;
class HeapWrapperItem;

class RenderTargetBackBuffer : public iRenderTarget
{
public:
   RenderTargetBackBuffer(
      DrawSystem* const pDrawSystem,
      const int bufferIndex,
      const RenderTargetFormatData& targetFormatData,
      const RenderTargetDepthData& targetDepthData
      );

private:
   virtual void OnDeviceLost() override;
   virtual void OnDeviceRestored(
      CustomCommandList* const pCommandList,
      ID3D12Device* const pDevice
      ) override;

   virtual void StartRender(ID3D12GraphicsCommandList* const pCommandList) override;
   virtual void EndRender(ID3D12GraphicsCommandList* const pCommandList) override;

   virtual void GetFormatData(
      DXGI_FORMAT& depthFormat,
      int& renderTargetViewFormatCount,
      const DXGI_FORMAT*& pRenderTargetViewFormat
      ) const override;

private:
   int m_bufferIndex;
   RenderTargetFormatData m_targetFormatData;
   D3D12_CLEAR_VALUE m_targetClearValue;
   RenderTargetDepthData m_targetDepthData;
   D3D12_CLEAR_VALUE m_depthClearValue;

   Microsoft::WRL::ComPtr<ID3D12Resource> m_pRenderTargetResource;
   std::shared_ptr<HeapWrapperItem> m_pRenderTargetDescriptor;

   Microsoft::WRL::ComPtr<ID3D12Resource> m_pDepthResource;
   std::shared_ptr<HeapWrapperItem> m_pDepthStencilViewDescriptor;

   D3D12_RESOURCE_STATES m_currentState;

};

#pragma once
#include "Common/Render/iResource.h"

class iRenderTarget : public iResource
{
public:
   iRenderTarget(DrawSystem* const pDrawSystem);
   virtual ~iRenderTarget();

   virtual void StartRender(ID3D12GraphicsCommandList* const pCommandList) = 0;
   virtual void EndRender(ID3D12GraphicsCommandList* const pCommandList) = 0;

   //virtual void GetPipelineState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc) const = 0;
   //virtual DXGI_FORMAT GetDepthFormat() const;
   virtual void GetFormatData(
      DXGI_FORMAT& depthFormat,
      int& renderTargetViewFormatCount,
      const DXGI_FORMAT*& pRenderTargetViewFormat
      ) const = 0;

};

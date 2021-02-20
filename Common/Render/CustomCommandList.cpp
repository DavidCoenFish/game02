#include "CommonPCH.h"

#include "Common/Render/CustomCommandList.h"
#include "Common/Render/Shader.h"
#include "Common/Render/Geometry.h"
#include "Common/Render/iRenderTarget.h"
#include "Common/Render/ShaderTexture2D.h"

CustomCommandList::CustomCommandList(
   ID3D12Device* const pDevice,
   ID3D12CommandQueue* const pCommandQueue,
   ID3D12GraphicsCommandList* const pCommandList,
   iRenderTarget* const pRenderTargetOrNull,
   const int fameIndex
   )
   : m_pDevice(pDevice)
   , m_pCommandQueue(pCommandQueue)
   , m_pCommandList(pCommandList)
   , m_pRenderTarget(nullptr)
   , m_frameIndex(fameIndex)
{
   SetRenderTarget(pRenderTargetOrNull);
   return;
}

CustomCommandList::~CustomCommandList()
{
   if (m_pRenderTarget)
   {
      m_pRenderTarget->EndRender(m_pCommandList);
   }

   // Send the command list off to the GPU for processing.
   if (m_pCommandList)
   {
      DX::ThrowIfFailed(m_pCommandList->Close());
   }
   if (m_pCommandQueue)
   {
      m_pCommandQueue->ExecuteCommandLists(1, CommandListCast(&m_pCommandList));
   }

   return;
}

void CustomCommandList::SetRenderTarget(iRenderTarget* const pRenderTarget)
{
   if (m_pRenderTarget == pRenderTarget)
   {
      return;
   }
   if (m_pRenderTarget)
   {
      m_pRenderTarget->EndRender(m_pCommandList);
   }
   m_pRenderTarget = pRenderTarget;
   if (m_pRenderTarget)
   {
      m_pRenderTarget->StartRender(m_pCommandList);
   }
   return;
}

void CustomCommandList::Draw(
   Geometry* const pGeometry, 
   Shader* const pShader
   )
{
   if ((nullptr == m_pCommandList) || (nullptr == pGeometry) || (nullptr == pShader))
   {
      return;
   }

   pShader->SetActivate(m_pCommandList, m_frameIndex);
   pGeometry->Draw(m_pCommandList);

   return;
}

void CustomCommandList::DrawShader(
   Shader* const pShader
   )
{
   if ((nullptr == m_pCommandList) || (nullptr == pShader))
   {
      return;
   }

   pShader->SetActivate(m_pCommandList, m_frameIndex);
   return;
}

void CustomCommandList::DrawGeometry(
   Geometry* const pGeometry
   )
{
   if ((nullptr == m_pCommandList) || (nullptr == pGeometry))
   {
      return;
   }

   pGeometry->Draw(m_pCommandList);
   return;
}

void CustomCommandList::UpdateSubresourcesMethod(
   ID3D12Resource* pDestinationResource,
   ID3D12Resource* pIntermediate,
   UINT64 IntermediateOffset,
   UINT FirstSubresource,
   UINT NumSubresources,
   D3D12_SUBRESOURCE_DATA* pSrcData,
   D3D12_BOX* pDestBoxOrNull
   )
{
   UpdateSubresources(
      m_pCommandList,
      pDestinationResource,
      pIntermediate,
      IntermediateOffset,
      FirstSubresource,
      NumSubresources,
      pSrcData,
      pDestBoxOrNull
      );
   return;
}

void CustomCommandList::ResourceBarrier(
   ID3D12Resource* const pResource,
   D3D12_RESOURCE_STATES stateBefore,
   D3D12_RESOURCE_STATES stateAfter
   )
{
   auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pResource, stateBefore, stateAfter);
   m_pCommandList->ResourceBarrier(1, &barrier);
}


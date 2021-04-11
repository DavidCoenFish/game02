#include "CommonPCH.h"

#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/d3dx12.h"

DrawSystemFrame::DrawSystemFrame(DrawSystem& drawSystem)
   : m_drawSystem(drawSystem)
   , m_pCommandList(nullptr)
{
   m_drawSystem.Prepare(
      m_pCommandList
      );
   m_drawSystem.Clear();
}

DrawSystemFrame::~DrawSystemFrame()
{
   //if (m_pCommandList)
   //{
   //   DX::ThrowIfFailed(m_pCommandList->Close());
   //}
   //if (m_pCommandList && m_pCommandQueue)
   //{
   //   m_pCommandQueue->ExecuteCommandLists(1, CommandListCast(m_pCommandList.GetAddressOf()));
   //}

   m_drawSystem.Present();
}

ID3D12GraphicsCommandList* DrawSystemFrame::GetCommandList()
{
   return m_pCommandList;
}

const int DrawSystemFrame::GetBackBufferIndex()
{
   return m_drawSystem.GetBackBufferIndex();
}

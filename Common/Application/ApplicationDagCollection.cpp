#include "CommonPCH.h"

#include "Common/Application/ApplicationDagCollection.h"
#include "Common/Dag/DagNodeValue.h"
#include "Common/Dag/DagValue.h"
#include "Common/Math/VectorInt2.h"

const std::string ApplicationDagCollection::GetDagNodeKeyFrameCount()
{
   return "_FrameCount";
}
const std::string ApplicationDagCollection::GetDagNodeKeyFrameMilliseconds()
{
   return "_FrameMilliseconds";
}
const std::string ApplicationDagCollection::GetDagNodeKeyWindowSize()
{
   return "_WindowSize";
}
const std::string ApplicationDagCollection::GetDagNodeKeyBackBuffer()
{
   return "_BackBuffer";
}
const std::string ApplicationDagCollection::GetDagNodeKeyCustomCommandList()
{
   return "_CustomCommandList";
}
const std::string ApplicationDagCollection::GetDagNodeKeyRender()
{
   return "_Render";
}



std::unique_ptr< ApplicationDagCollection > ApplicationDagCollection::Factory(const int width, const int height)
{
   auto pResult = std::make_unique< ApplicationDagCollection >(width, height);
   return pResult;
}

ApplicationDagCollection::ApplicationDagCollection(const int width, const int height)
{
   {
      auto pTemp = DagValue<int>::Factory(0);
      m_pDagFrameCount = DagNodeValue::Factory( pTemp );
      AddDagNode(GetDagNodeKeyFrameCount(), m_pDagFrameCount);
   }
   {
      auto pTemp = DagValue<long long>::Factory(0);
      m_pDagFrameMilliseconds = DagNodeValue::Factory( pTemp );
      AddDagNode(GetDagNodeKeyFrameMilliseconds(), m_pDagFrameMilliseconds);
   }
   {
      auto pTemp = DagValue<VectorInt2>::Factory(VectorInt2(width, height));
      m_pDagWindowSize = DagNodeValue::Factory( pTemp );
      AddDagNode(GetDagNodeKeyWindowSize(), m_pDagWindowSize);
   }
   {
      auto pTemp = DagValue<iRenderTarget*>::Factory(nullptr);
      m_pDagBackBuffer = DagNodeValue::Factory( pTemp );
      AddDagNode(GetDagNodeKeyBackBuffer(), m_pDagBackBuffer);
   }
   {
      auto pTemp = DagValue<CustomCommandList*>::Factory(nullptr);
      m_pDagCustomCommandList = DagNodeValue::Factory( pTemp, false );
      AddDagNode(GetDagNodeKeyCustomCommandList(), m_pDagCustomCommandList);
   }
   return;
}

void ApplicationDagCollection::SetWidthHeight(const int width, const int height)
{
   DagValue<VectorInt2>::UpdateNode(m_pDagWindowSize, VectorInt2(width, height));
}

void ApplicationDagCollection::Update(const long long milliseconds)
{
   DagValue<int>::IncrementNode(m_pDagFrameCount, 1);
   DagValue<long long>::UpdateNode(m_pDagFrameMilliseconds, milliseconds);
}

void ApplicationDagCollection::Render(CustomCommandList* const pCustomCommandList, iRenderTarget* const pRenderTarget)
{
   DagValue<CustomCommandList*>::UpdateNode(m_pDagCustomCommandList, pCustomCommandList);
   DagValue<iRenderTarget*>::UpdateNode(m_pDagBackBuffer, pRenderTarget);

   auto pRenderNode = GetDagNode(GetDagNodeKeyRender());
   if (pRenderNode)
   {
      pRenderNode->GetValue();
   }

   return;
}

#pragma once

#include "Common\DAG\dagcollection.h"
class iRenderTarget;
class iDagNode;
class CustomCommandList;

class ApplicationDagCollection : public DagCollection
{
public:
   static const std::string GetDagNodeKeyFrameCount();
   static const std::string GetDagNodeKeyFrameMilliseconds();
   static const std::string GetDagNodeKeyWindowSize();
   static const std::string GetDagNodeKeyBackBuffer();
   static const std::string GetDagNodeKeyCustomCommandList();
   static const std::string GetDagNodeKeyRender();

   static std::unique_ptr< ApplicationDagCollection > Factory(const int width, const int height);

   ApplicationDagCollection(const int width, const int height);

   void SetWidthHeight(const int width, const int height);
   void Update(const long long milliseconds);
   void Render(CustomCommandList* const pCustomCommandList, iRenderTarget* const pRenderTarget);

private:
   std::shared_ptr< iDagNode > m_pDagFrameCount;
   std::shared_ptr< iDagNode > m_pDagFrameMilliseconds;
   std::shared_ptr< iDagNode > m_pDagWindowSize;
   std::shared_ptr< iDagNode > m_pDagBackBuffer;
   std::shared_ptr< iDagNode > m_pDagCustomCommandList;
};

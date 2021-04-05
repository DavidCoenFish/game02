#include "CommonPCH.h"

#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/DrawSystem/DrawSystem.h"

DrawSystemFrame::DrawSystemFrame(DrawSystem& drawSystem)
   : m_drawSystem(drawSystem)
{
   m_drawSystem.Prepare();
   m_drawSystem.Clear();
}

DrawSystemFrame::~DrawSystemFrame()
{
   m_drawSystem.Present();
}

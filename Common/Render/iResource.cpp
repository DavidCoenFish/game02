#include "CommonPCH.h"

#include "Common/Render/iResource.h"
#include "Common/Render/DrawSystem.h"

iResource::iResource(DrawSystem* const pDrawSystem)
: m_pDrawSystem(pDrawSystem)
{
   if (nullptr != m_pDrawSystem)
   {
      m_pDrawSystem->AddResource(this);
   }
   return;
}
iResource::~iResource()
{
   if (nullptr != m_pDrawSystem)
   {
      m_pDrawSystem->RemoveResource(this);
   }
}

void iResource::OnResizeBefore()
{
   return;
}
void iResource::OnResizeAfter(
   CustomCommandList* const,
   ID3D12Device* const,
   const int,
   const int
   )
{
   return;
}



#include "CommonPCH.h"

#include "Common/Render/iRenderTarget.h"
#include "Common/Render/iResource.h"

iRenderTarget::iRenderTarget(DrawSystem* const pDrawSystem)
   : iResource(pDrawSystem)
{
   return;
}

iRenderTarget::~iRenderTarget()
{
   return;
}


#include "CommonPCH.h"

#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/DeviceResources.h"
#include "Common/DrawSystem/DrawSystemFrame.h"

DrawSystem::DrawSystem(
   const HWND hWnd,
   const D3D_FEATURE_LEVEL d3dFeatureLevel,
   const unsigned int options
   )
   : m_hWnd(hWnd)
   , m_d3dFeatureLevel(d3dFeatureLevel)
   , m_options(options)
{
   CreateDeviceResources();
}

DrawSystem::~DrawSystem()
{
   //nop
}


std::unique_ptr< DrawSystemFrame > DrawSystem::CreateNewFrame()
{
   return nullptr;
}

void DrawSystem::WaitForGpu() noexcept
{
}

void DrawSystem::OnResize()
{
}

void DrawSystem::CreateDeviceResources()
{
   m_pDeviceResources.reset();
   m_pDeviceResources = std::make_unique< DeviceResources >(
      m_hWnd,
      m_d3dFeatureLevel,
      m_options
      );
}



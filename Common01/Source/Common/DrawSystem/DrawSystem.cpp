#include "CommonPCH.h"

#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/DeviceResources.h"
#include "Common/DrawSystem/DrawSystemFrame.h"

DrawSystem::DrawSystem(
   const HWND hWnd,
   const unsigned int backBufferCount,
   const D3D_FEATURE_LEVEL d3dFeatureLevel,
   const unsigned int options
   )
   : m_hWnd(hWnd)
   , m_backBufferCount(backBufferCount)
   , m_d3dFeatureLevel(d3dFeatureLevel)
   , m_options(options)
{
   CreateDeviceResources();
}

DrawSystem::~DrawSystem()
{
   WaitForGpu();
}


std::unique_ptr< DrawSystemFrame > DrawSystem::CreateNewFrame()
{
   return std::make_unique< DrawSystemFrame >( *this );
}

void DrawSystem::Prepare()
{
   if (nullptr == m_pDeviceResources)
   {
      return;
   }

   m_pDeviceResources->Prepare();
}

void DrawSystem::Clear()
{
   if (nullptr == m_pDeviceResources)
   {
      return;
   }
   m_pDeviceResources->Clear();
}

void DrawSystem::Present()
{
   if (nullptr == m_pDeviceResources)
   {
      return;
   }

   if (false == m_pDeviceResources->Present())
   {
      CreateDeviceResources();
   }
}

void DrawSystem::WaitForGpu() noexcept
{
   if (m_pDeviceResources)
   {
      m_pDeviceResources->WaitForGpu();
   }
}

void DrawSystem::OnResize()
{
   if (m_pDeviceResources)
   {
      m_pDeviceResources->OnResize(m_hWnd);
   }
}

void DrawSystem::CreateDeviceResources()
{
   m_pDeviceResources.reset();
   m_pDeviceResources = std::make_unique< DeviceResources >(
      m_hWnd,
      2,
      m_d3dFeatureLevel,
      m_options
      );
}



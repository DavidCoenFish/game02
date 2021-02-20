//
// Main.cpp
//

#include "PCH.h"
#include "Application.h"
#include "Common/JSON/JSONApplicationDagCollection.h"
#include "Common/JSON/JSONDrawSystem.h"
#include "Common/JSON/JSONGeometry.h"
#include "Common/JSON/JSONShader.h"
#include "Common/Render/DrawSystem.h"
#include "Common/Render/CustomCommandList.h"
#include "Common/Render/ShaderPipelineStateData.h"
#include "Common/Application/ApplicationDagCollection.h"
#include "Common/Application/ApplicationDagCalculate.h"
#include "Common/Application/FileCache.h"
#include "SDK/json/json.hpp"

Application::Application(HWND hwnd, const int width, const int height)
   : m_bTicked(false)
{
   m_pFileCache = std::make_unique< FileCache >();
   //auto applicationJson = nlohmann::json::parse( FileCache::RawLoadFileString(L"Resources\\DAG\\SimpleTriangleApplication.json"));
   auto applicationJson = nlohmann::json::parse( FileCache::RawLoadFileString(L"Resources\\DAG\\DepthTriangleApplication.json"));
   m_pDrawSystem = JSONDrawSystem::Factory(
      []()
      {
         PostQuitMessage(-1);
         return;
      },
      applicationJson,
      hwnd, 
      width,
      height
   );

   ApplicationDagCalculate::SetDefaultFunctions();
   {
      auto pCommandList = m_pDrawSystem ? m_pDrawSystem->MakeCommandList() : nullptr;

      m_pApplicationDagCollection = JSONApplicationDagCollection::Factory(
         m_pDrawSystem.get(),
         pCommandList.get(),
         m_pFileCache.get(),
         applicationJson,
         width, 
         height
         );
   }
   if (m_pDrawSystem)
   {
      m_pDrawSystem->WaitForGpu();
   }
}

Application::~Application()
{
   m_pApplicationDagCollection.reset();
   m_pDrawSystem.reset();
   return;
}

void Application::Tick()
{
   m_timePoint = std::chrono::steady_clock::now();
   if (false == m_bTicked)
   {
      m_bTicked = true;
   }
   if (m_pApplicationDagCollection)
   {
      long long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(m_timePoint.time_since_epoch()).count();
      m_pApplicationDagCollection->Update(milliseconds);
   }
   Render();
}

void Application::OnWindowMoved()
{
}
void Application::OnSuspending()
{
}
void Application::OnResuming()
{
}
void Application::OnWindowSizeChanged(const int width, const int height)
{
   if (m_pDrawSystem)
   {
      m_pDrawSystem->WindowSizeChanged(width, height);
   }
   if (m_pApplicationDagCollection)
   {
      m_pApplicationDagCollection->SetWidthHeight(width, height);
   }
}
void Application::OnActivated()
{
}
void Application::OnDeactivated()
{
}

void Application::Render()
{
   {
      auto pCommandList = m_pDrawSystem ? m_pDrawSystem->MakeCommandList() : nullptr;
      if (nullptr == pCommandList)
      {
         return;
      }

      if (m_pApplicationDagCollection)
      {
         auto pBackBuffer = m_pDrawSystem->GetCurrentRenderTargetBackbuffer();
         m_pApplicationDagCollection->Render(pCommandList.get(), pBackBuffer);
      }
   }

   if (m_pDrawSystem)
   {
      m_pDrawSystem->Present();
   }
}

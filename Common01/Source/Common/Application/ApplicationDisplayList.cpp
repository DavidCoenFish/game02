#include "CommonPCH.h"
#include "Common/Application/ApplicationDisplayList.h"
#include "Common/Log/Log.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/FileSystem/FileSystem.h"
#include "Common/DrawSystem/CustomCommandList.h"
#include "Common/DrawSystem/Shader/ShaderPipelineStateData.h"
#include "Common/DrawSystem/Geometry/GeometryGeneric.h"
#include "Common/DrawSystem/Shader/Shader.h"
#include "Common/JSON/JSONDrawSystem.h"
#include "json/json.hpp"


class JSONData
{
public:
   JSONDrawSystem drawSystem;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONData,
   drawSystem
   );


ApplicationDisplayList::ApplicationDisplayList(const IApplicationParam& applicationParam, const std::filesystem::path& rootPath, const std::string& data)
   : IApplication(applicationParam)
{
   LOG_MESSAGE("ApplicationDisplayList ctor %p", this);

   std::filesystem::path dataPath = rootPath / data;
   auto pFile = FileSystem::GetFileString(dataPath);
   auto json = nlohmann::json::parse( pFile ? *pFile : "{}");
   JSONData jsonData;
   json.get_to(jsonData);

   m_pDrawSystem = std::make_unique< DrawSystem>(
      applicationParam.m_hWnd,
      jsonData.drawSystem.backBufferCount,
      jsonData.drawSystem.d3dFeatureLevel,
      jsonData.drawSystem.options,
      jsonData.drawSystem.targetFormatData,
      jsonData.drawSystem.targetDepthData
      );
}

ApplicationDisplayList::~ApplicationDisplayList()
{
   if (m_pDrawSystem)
   {
      m_pDrawSystem->WaitForGpu();
   }

   LOG_MESSAGE("ApplicationDisplayList dtor %p", this);
}

void ApplicationDisplayList::Update()
{
   BaseType::Update();
   if (m_pDrawSystem)
   {
      auto pFrame = m_pDrawSystem->CreateNewFrame();
      pFrame->SetRenderTarget(m_pDrawSystem->GetRenderTargetBackBuffer());
   }
}

void ApplicationDisplayList::OnWindowSizeChanged(const int width, const int height)
{
   BaseType::OnWindowSizeChanged(width, height);
   if (m_pDrawSystem)
   {
      m_pDrawSystem->OnResize();
   }

   return;
}

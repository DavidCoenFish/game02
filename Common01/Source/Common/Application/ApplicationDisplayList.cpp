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
#include "Common/JSON/JSONDagCollection.h"
#include "Common/DAG/DagCollection.h"
#include "json/json.hpp"


class JSONData
{
public:
   JSONDrawSystem drawSystem;
   JSONDagCollection dagCollection;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONData,
   drawSystem,
   dagCollection
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
   {
      std::map<std::string, std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)>> mapValue;
      std::map<std::string, std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)>> mapCalculate;
      //mapValue["shader"] = [=](const nlohmann::json& data){
      //   auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
      //   return m_pDrawSystem->MakeShader(
      //      pCommandList->GetCommandList()
      //      );
      //};


      m_pDagCollection = JSONDagCollection::Factory(jsonData.dagCollection, mapValue, mapCalculate);
   }

}

ApplicationDisplayList::~ApplicationDisplayList()
{
   if (m_pDrawSystem)
   {
      m_pDrawSystem->WaitForGpu();
   }
   m_pDagCollection.reset();
   m_pDrawSystem.reset();

   LOG_MESSAGE("ApplicationDisplayList dtor %p", this);
}

void ApplicationDisplayList::Update()
{
   BaseType::Update();
   if (m_pDrawSystem)
   {
      auto pFrame = m_pDrawSystem->CreateNewFrame();
      pFrame->SetRenderTarget(m_pDrawSystem->GetRenderTargetBackBuffer());

      //m_pDagCollection->
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

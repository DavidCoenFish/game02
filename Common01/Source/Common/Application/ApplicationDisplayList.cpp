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
#include "Common/JSON/JSONShader.h"
#include "Common/DAG/DagCollection.h"
#include "Common/DAG/DagValue.h"
#include "Common/DAG/DagNodeValue.h"
#include "Common/DAG/iDagNode.h"
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

std::shared_ptr< std::vector<uint8_t> > LoadFile(const std::filesystem::path& rootPath, const std::string& fileName)
{
   if (true == fileName.empty())
   {
      return nullptr;
   }
   auto newPath = rootPath / fileName;
   auto pResult = FileSystem::ReadFileLoadData(newPath, true);
   return pResult;
}

typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> NodeValueFactory;

ApplicationDisplayList::ApplicationDisplayList(const IApplicationParam& applicationParam, const std::filesystem::path& rootPath, const std::string& data)
   : IApplication(applicationParam)
{
   LOG_MESSAGE("ApplicationDisplayList ctor %p", this);

   std::filesystem::path dataPath = rootPath / data;
   auto fileString = FileSystem::DataToString(FileSystem::ReadFileLoadData(dataPath));
   auto json = nlohmann::json::parse( fileString );
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
      std::map<std::string, NodeValueFactory> mapValue;
      mapValue["Shader"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
         JSONShader jsonShader;
         data.get_to(jsonShader);
         auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
         auto pShader = m_pDrawSystem->MakeShader(
            pCommandList->GetCommandList(),
            jsonShader.pipelineState,
            LoadFile(rootPath, jsonShader.vertexShader),
            LoadFile(rootPath, jsonShader.geometryShader),
            LoadFile(rootPath, jsonShader.pixelShader)
            );
         auto pValue = DagValue<std::shared_ptr< Shader >>::Factory(pShader);
         auto pResult = DagNodeValue::Factory( pValue, false );
         return pResult;
      };

      std::map<std::string, std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)>> mapCalculate;
      //mapValue.insert(std::pair<std::string, std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)>>( 
      //   "shader",
         
         //const std::vector< std::shared_ptr< ShaderResourceInfo > >& arrayShaderResourceInfo = std::vector< std::shared_ptr< ShaderResourceInfo > >(),
         //const std::vector< std::shared_ptr< ShaderConstantInfo > >& arrayShaderConstantsInfo = std::vector< std::shared_ptr< ShaderConstantInfo > >()


      //mapCalculate

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

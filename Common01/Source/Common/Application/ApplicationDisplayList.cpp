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
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/JSON/JSONDrawSystem.h"
#include "Common/JSON/JSONDagCollection.h"
#include "Common/JSON/JSONShader.h"
#include "Common/JSON/JSONGeometry.h"
#include "Common/DAG/DagCollection.h"
#include "Common/DAG/DagValue.h"
#include "Common/DAG/DagNodeValue.h"
#include "Common/DAG/DagNodeCalculate.h"
#include "Common/DAG/iDagNode.h"
#include "json/json.hpp"

/*
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
typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> NodeCalculateFactory;

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
      mapValue["Geometry"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
         JSONGeometry jsonGeometry;
         data.get_to(jsonGeometry);
         auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
         auto pGeometry = m_pDrawSystem->MakeGeometryGeneric(
            pCommandList->GetCommandList(),
            jsonGeometry.topology,
            jsonGeometry.inputElementDesc,
            jsonGeometry.vertexData,
            jsonGeometry.floatPerVertex
            );
         auto pValue = DagValue<std::shared_ptr< GeometryGeneric >>::Factory(pGeometry);
         auto pResult = DagNodeValue::Factory( pValue, false );
         return pResult;
      };
      mapValue["Texture2D"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
         JSONGeometry jsonGeometry;
         data.get_to(jsonGeometry);
         auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
         auto pGeometry = m_pDrawSystem->MakeGeometryGeneric(
            pCommandList->GetCommandList(),
            jsonGeometry.topology,
            jsonGeometry.inputElementDesc,
            jsonGeometry.vertexData,
            jsonGeometry.floatPerVertex
            );
         auto pValue = DagValue<std::shared_ptr< GeometryGeneric >>::Factory(pGeometry);
         auto pResult = DagNodeValue::Factory( pValue, false );
         return pResult;
      };


      std::map<std::string, NodeCalculateFactory> mapCalculate;
      mapCalculate["GeometryList"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
         data;
         auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue >{
            stackInput; indexInput; pValue;
            auto pLocalValue = std::dynamic_pointer_cast<DagValue< std::vector< std::shared_ptr< GeometryGeneric > > >>(pValue);
            if (nullptr == pLocalValue)
            {
               //pLocalValue = std::make_shared< DagValue< std::vector< std::shared_ptr< Geometry > > > >();
               pLocalValue = DagValue< std::vector< std::shared_ptr< GeometryGeneric > > >::Factory(std::vector< std::shared_ptr< GeometryGeneric > >());
            }
            std::vector< std::shared_ptr< GeometryGeneric > > resultData;
            for (const auto& item : stackInput )
            {
               auto pTemp = item ? std::dynamic_pointer_cast< DagValue< std::shared_ptr< GeometryGeneric > > >(item->GetValue()) : nullptr;
               if (nullptr == pTemp)
               {
                  continue;
               }
               resultData.push_back(pTemp->GetRef());
            }
            pLocalValue->SetRef(resultData);
            return pLocalValue;
         });
         return pResult;
      };

      mapCalculate["Draw"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
         data;
         auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& orderedInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue >{
            stackInput; orderedInput; pValue;
            typedef std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > > PairData;
            auto pLocalValue = std::dynamic_pointer_cast<DagValue< PairData > >(pValue);

            const auto pShader = std::dynamic_pointer_cast< DagValue< std::shared_ptr< Shader > > >(orderedInput[0]->GetValue());
            const auto pGeometryList = std::dynamic_pointer_cast< DagValue< std::vector< std::shared_ptr< GeometryGeneric > > > >(orderedInput[1]->GetValue());
            PairData newValue( pShader->GetRef(), pGeometryList->GetRef() );
            if (nullptr == pLocalValue)
            {
               //pLocalValue = std::make_shared< DagValue< std::vector< std::shared_ptr< Geometry > > > >();
               pLocalValue = DagValue< PairData >::Factory(newValue);
            }
            else
            {
               pLocalValue->SetRef(newValue);
            }
            return pLocalValue;
         });
         return pResult;
      };

      mapCalculate["DrawList"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
         data;
         auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& orderedInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue >{
            stackInput; orderedInput; pValue;
            typedef std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > > PairData;
            typedef std::vector< PairData > PairArray;
            auto pLocalValue = std::dynamic_pointer_cast<DagValue< PairArray >>(pValue);
            if (nullptr == pLocalValue)
            {
               PairArray dummy;
               pLocalValue = DagValue< PairArray >::Factory(PairArray());
            }
            PairArray resultData;
            for (const auto& item : stackInput )
            {
               //auto pTemp = std::dynamic_pointer_cast< DagValue< std::shared_ptr< Geometry > > >(item);
               auto pTemp = item ? std::dynamic_pointer_cast< DagValue< PairData > >(item->GetValue()) : nullptr;
               if (nullptr == pTemp)
               {
                  continue;
               }
               resultData.push_back(pTemp->GetRef());
            }
            pLocalValue->SetRef(resultData);
            return pLocalValue;
         });
         return pResult;
      };

      mapCalculate["RenderList"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
         data;
         auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& orderedInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue >{
            stackInput; orderedInput; pValue;
            typedef std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > > PairData;
            typedef std::vector< PairData > PairArray;

            const auto pDAGDrawSystemFrame = std::dynamic_pointer_cast< DagValue< DrawSystemFrame* > >(orderedInput[1]->GetValue());
            const auto pDAGRenderTarget = std::dynamic_pointer_cast< DagValue< IRenderTarget* > >(orderedInput[2]->GetValue());
            const auto pDAGDrawList = std::dynamic_pointer_cast< DagValue< PairArray > >(orderedInput[3]->GetValue());

            DrawSystemFrame* pDrawSystemFrame = pDAGDrawSystemFrame ? pDAGDrawSystemFrame->Get() : nullptr;
            IRenderTarget* pRenderTarget = pDAGRenderTarget ? pDAGRenderTarget->Get() : nullptr;
            assert(nullptr != pDrawSystemFrame);
            assert(nullptr != pRenderTarget);
            assert(nullptr != pDAGDrawList);
            const auto& drawList = pDAGDrawList->GetRef();

            pDrawSystemFrame->SetRenderTarget( pRenderTarget );

            for (const auto& item : drawList)
            {
               pDrawSystemFrame->SetShader(item.first.get());
               for (const auto& subItem : item.second)
               {
                  pDrawSystemFrame->Draw(subItem.get());
               }
            }

            return pValue;
         });
         return pResult;
      };

      std::vector< std::pair< std::string, std::shared_ptr< iDagNode > > > inbuiltDagValues;
      {
         //_FrameCount
         m_pDagFrameCount = DagNodeValue::Factory( DagValue<int>::Factory(0));
         inbuiltDagValues.push_back( std::pair< std::string, std::shared_ptr< iDagNode > >(
            "_FrameCount", m_pDagFrameCount ));
         //_DrawSystemFrame
         m_pDagDrawSystemFrame = DagNodeValue::Factory( 
               DagValue<DrawSystemFrame*>::Factory(nullptr),
               false
               );
         inbuiltDagValues.push_back( std::pair< std::string, std::shared_ptr< iDagNode > >(
            "_DrawSystemFrame", m_pDagDrawSystemFrame));
         //_BackBuffer
         m_pDagBackBuffer = DagNodeValue::Factory(DagValue<IRenderTarget*>::Factory(nullptr));
         inbuiltDagValues.push_back( std::pair< std::string, std::shared_ptr< iDagNode > >(
            "_BackBuffer", m_pDagBackBuffer));
      }

      m_pDagCollection = JSONDagCollection::Factory(jsonData.dagCollection, inbuiltDagValues, mapValue, mapCalculate);
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

      DagValue<int>::IncrementNode(m_pDagFrameCount, 1);
      DagValue<DrawSystemFrame*>::UpdateNode(m_pDagDrawSystemFrame, pFrame.get());
      DagValue<IRenderTarget*>::UpdateNode(m_pDagBackBuffer, m_pDrawSystem->GetRenderTargetBackBuffer());

      auto pRender = m_pDagCollection->GetDagNode("_Render");
      if (pRender)
      {
         pRender->GetValue();
      }
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
*/
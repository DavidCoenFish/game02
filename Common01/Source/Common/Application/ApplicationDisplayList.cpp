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
#include "Common/JSON/JSONRenderTarget.h"
#include "Common/DAG/DagCollection.h"
#include "Common/DAG/DagValue.h"
#include "Common/DAG/DagNodeValue.h"
#include "Common/DAG/DagNodeCalculate.h"
#include "Common/DAG/iDagNode.h"
#include "json/json.hpp"

class JSONData
{
public:
   JSONDrawSystem drawSystem;
   std::string file;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONData,
   drawSystem,
   file
   );

std::shared_ptr< std::vector<uint8_t> > LoadFile(const std::filesystem::path& rootPath, const std::string& fileName)
{
   if (true == fileName.empty())
   {
      return nullptr;
   }
   auto newPath = rootPath / fileName;
   auto pResult = FileSystem::SyncReadFile(newPath);
   return pResult;
}

typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> NodeValueFactory;
typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> NodeCalculateFactory;

IApplication* const ApplicationDisplayList::Factory(const HWND hWnd, const IApplicationParam& applicationParam)
{
   return new ApplicationDisplayList(hWnd, applicationParam);
}

ApplicationDisplayList::ApplicationDisplayList(const HWND hWnd, const IApplicationParam& applicationParam)
   : IApplication(hWnd, applicationParam)
   , m_timePointValid(false)
   , m_timePoint()
{
   LOG_MESSAGE("ApplicationDisplayList ctor %p", this);

   JSONData jsonData;
   nlohmann::from_json(applicationParam.m_json, jsonData);

   m_pDrawSystem = DrawSystem::Factory(hWnd, jsonData.drawSystem);

   std::map<std::string, NodeValueFactory> mapValue;
   mapValue["Shader"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
      JSONShader jsonShader;
      data.get_to(jsonShader);
      auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
      auto pShader = m_pDrawSystem->MakeShader(
         pCommandList->GetCommandList(),
         jsonShader.pipelineState,
         LoadFile(applicationParam.m_rootPath, jsonShader.vertexShader),
         LoadFile(applicationParam.m_rootPath, jsonShader.geometryShader),
         LoadFile(applicationParam.m_rootPath, jsonShader.pixelShader)
         );
      auto pValue = DagValue<std::shared_ptr< Shader >>::Factory(pShader);
      auto pResult = DagNodeValue::Factory( pValue );
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
      auto pResult = DagNodeValue::Factory( pValue );
      return pResult;
   };
   mapValue["RenderTarget"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
      JSONRenderTarget jsonRenderTarget;
      data.get_to(jsonRenderTarget);
      auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
      auto pRenderTargetTexture = m_pDrawSystem->MakeRenderTargetTexture(
         pCommandList->GetCommandList(),
         jsonRenderTarget.targetFormatDataArray,
         jsonRenderTarget.targetDepthData,
         jsonRenderTarget.width,
         jsonRenderTarget.height,
         jsonRenderTarget.resizeWithScreen
         );
      auto pValue = DagValue<std::shared_ptr< RenderTargetTexture >>::Factory(pRenderTargetTexture);
      auto pResult = DagNodeValue::Factory( pValue );
      return pResult;
   };

   std::map<std::string, NodeCalculateFactory> mapCalculate;
   //mapCalculate["Shader"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
   //   //0.input array of shader 
   //   //1.input array of array of const buffer floats 
   //};

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

         auto pLocalValue = std::dynamic_pointer_cast<DagValue< IRenderTarget* >>(pValue);
         if (nullptr == pLocalValue)
         {
            pLocalValue = DagValue< IRenderTarget* >::Factory(nullptr);
         }
         pLocalValue->SetRef(pRenderTarget);

         return pLocalValue;
      });
      return pResult;
   };

   std::vector< std::pair< std::string, std::shared_ptr< iDagNode > > > inbuiltDagValues;
   {
      //_FrameCount
      m_pDagFrameCount = DagNodeValue::Factory( DagValue<int>::Factory(0));
      inbuiltDagValues.push_back( {"_FrameCount", m_pDagFrameCount });
      //_DrawSystemFrame
      m_pDagDrawSystemFrame = DagNodeValue::Factory( 
            DagValue<DrawSystemFrame*>::Factory(nullptr),
            false
            );
      inbuiltDagValues.push_back( {"_DrawSystemFrame", m_pDagDrawSystemFrame});
      //_BackBuffer
      m_pDagBackBuffer = DagNodeValue::Factory(DagValue<IRenderTarget*>::Factory(nullptr));
      inbuiltDagValues.push_back( {"_BackBuffer", m_pDagBackBuffer});
      //_TimeAccumulate
      m_pDagTimeAccumulate = DagNodeValue::Factory( DagValue<float>::Factory(0));
      inbuiltDagValues.push_back( {"_TimeAccumulate", m_pDagTimeAccumulate });
   }

   m_pDagCollection = JSONDagCollection::Factory(
      jsonData.file,
      [=](const std::string& fileName){
         auto newPath = applicationParam.m_rootPath / fileName;
         auto pResult = FileSystem::SyncReadFile(newPath);
         auto result = FileSystem::DataToString(pResult);
         return result;
      },
      inbuiltDagValues, 
      mapValue, 
      mapCalculate
      );
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

   const auto timePointNow = std::chrono::system_clock::now();
   float timeDeltaSeconds = 0.0f;
   if (true == m_timePointValid)
   {
      timeDeltaSeconds = (1.0f / 1000.0f) * (float)(std::chrono::duration_cast<std::chrono::milliseconds>(timePointNow - m_timePoint).count());
   } 
   else
   {
      m_timePointValid = true;
   }
   m_timePoint = timePointNow;
   if (m_pDagTimeAccumulate)
   {
      DagValue<float>::IncrementNode(m_pDagTimeAccumulate, timeDeltaSeconds);
   }

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

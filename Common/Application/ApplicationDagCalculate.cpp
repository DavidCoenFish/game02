#include "CommonPCH.h"

#include "Common/Application/ApplicationDagCalculate.h"
#include "Common/Dag/DagNodeValue.h"
#include "Common/Dag/DagValue.h"
#include "Common/Math/VectorInt2.h"
#include "Common/Render/Geometry.h"
#include "Common/Render/Shader.h"
#include "Common/Render/iRenderTarget.h"
#include "Common/Render/CustomCommandList.h"

static std::map< std::string, ApplicationDagCalculate::CalculateFunction >& GetFunctionMap()
{
   static std::map< std::string, ApplicationDagCalculate::CalculateFunction >s_functionMap;
   return s_functionMap;
}

void ApplicationDagCalculate::AddFunction(const std::string& key, const CalculateFunction& calculateFunction)
{
   GetFunctionMap()[key] = calculateFunction;
}


const ApplicationDagCalculate::CalculateFunction ApplicationDagCalculate::FindFunction(const std::string& key)
{
   auto& s_functionMap = GetFunctionMap();
   const auto iter = s_functionMap.find(key);
   if (iter != s_functionMap.end())
   {
      return iter->second;
   }
   return nullptr;
}

void ApplicationDagCalculate::SetDefaultFunctions()
{
   //stackInput
   AddFunction("GeometryList", [](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >&, const std::shared_ptr< iDagValue >& pValue){
         auto pLocalValue = std::dynamic_pointer_cast<DagValue< std::vector< std::shared_ptr< Geometry > > >>(pValue);
         if (nullptr == pLocalValue)
         {
            //pLocalValue = std::make_shared< DagValue< std::vector< std::shared_ptr< Geometry > > > >();
            pLocalValue = DagValue< std::vector< std::shared_ptr< Geometry > > >::Factory(std::vector< std::shared_ptr< Geometry > >());
         }
         std::vector< std::shared_ptr< Geometry > > resultData;
         for (const auto& item : stackInput )
         {
            auto pTemp = item ? std::dynamic_pointer_cast< DagValue< std::shared_ptr< Geometry > > >(item->GetValue()) : nullptr;
            if (nullptr == pTemp)
            {
               continue;
            }
            resultData.push_back(pTemp->GetRef());
         }
         pLocalValue->SetRef(resultData);
         return pLocalValue;
      });

   /*
      orderedInput
            "ShaderSimple",
            "GeometryList0"
   */
   AddFunction("Draw", [](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& orderedInput, const std::shared_ptr< iDagValue >& pValue){
      stackInput; orderedInput; pValue;
         typedef std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< Geometry > > > PairData;
         auto pLocalValue = std::dynamic_pointer_cast<DagValue< PairData > >(pValue);

         const auto pShader = std::dynamic_pointer_cast< DagValue< std::shared_ptr< Shader > > >(orderedInput[0]->GetValue());
         const auto pGeometryList = std::dynamic_pointer_cast< DagValue< std::vector< std::shared_ptr< Geometry > > > >(orderedInput[1]->GetValue());
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
   /*
      stackInput
   */
   AddFunction("DrawList", [](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& orderedInput, const std::shared_ptr< iDagValue >& pValue){
      stackInput; orderedInput; pValue;
         typedef std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< Geometry > > > PairData;
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

   /*
      orderedInput
            "_FrameCount",
            "_CustomCommandList",
            "_BackBuffer",
            "DrawList0"
   */
   AddFunction("RenderList", [](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& orderedInput, const std::shared_ptr< iDagValue >& pValue){
      stackInput; orderedInput; pValue;
         typedef std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< Geometry > > > PairData;
         typedef std::vector< PairData > PairArray;

         const auto pDAGCustomCommandList = std::dynamic_pointer_cast< DagValue< CustomCommandList* > >(orderedInput[1]->GetValue());
         const auto pDAGRenderTarget = std::dynamic_pointer_cast< DagValue< iRenderTarget* > >(orderedInput[2]->GetValue());
         const auto pDAGDrawList = std::dynamic_pointer_cast< DagValue< PairArray > >(orderedInput[3]->GetValue());

         CustomCommandList* pCommandList = pDAGCustomCommandList ? pDAGCustomCommandList->Get() : nullptr;
         iRenderTarget* pRenderTarget = pDAGRenderTarget ? pDAGRenderTarget->Get() : nullptr;
         assert(nullptr != pCommandList);
         assert(nullptr != pRenderTarget);
         assert(nullptr != pDAGDrawList);
         const auto& drawList = pDAGDrawList->GetRef();


         pCommandList->SetRenderTarget( pRenderTarget );

         for (const auto& item : drawList)
         {
            pCommandList->DrawShader(item.first.get());
            for (const auto& subItem : item.second)
            {
               pCommandList->DrawGeometry(subItem.get());
            }
         }

         return pValue;
      });
}

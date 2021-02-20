#include "CommonPCH.h"
#include "Common/JSON/JSONApplicationDagCollection.h"

#include "Common/Application/ApplicationDagCollection.h"
#include "Common/Application/ApplicationDagCalculate.h"
#include "Common/Application/FileCache.h"
#include "Common/DAG/DagValue.h"
#include "Common/DAG/DagNodeCalculate.h"
#include "Common/DAG/DagNodeValue.h"
#include "Common/JSON/JSONGeometry.h"
#include "Common/JSON/JSONShader.h"
#include "Common/Render/DrawSystem.h"
#include "Common/Render/Geometry.h"
#include "Common/Utils/Utf8.h"

enum class DagValueType
{
   Invalid = 0,
   Geometry,
   Shader
};

NLOHMANN_JSON_SERIALIZE_ENUM( DagValueType, {
   {DagValueType::Invalid, nullptr},
   {DagValueType::Geometry, "Geometry"},
   {DagValueType::Shader, "Shader"}
});

struct JSONDagValue
{
   std::string name;
   DagValueType type;
   nlohmann::json data;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JSONDagValue, name, type, data);

struct JSONDagCalculate
{
   std::string name;
   std::string function;
   nlohmann::json stackInput;
   nlohmann::json orderedInput;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JSONDagCalculate, name, function, stackInput, orderedInput);

static void DealDagValue(
   DrawSystem* const pDrawSystem, 
   CustomCommandList* const pCommandList,
   FileCache* const pFileCache,
   ApplicationDagCollection& applicationDagCollection, 
   const JSONDagValue& dagValue
   )
{
   switch(dagValue.type)
   {
   default:
      break;
   case DagValueType::Geometry:
      {
         auto pGeometry = JSONGeometry::Factory(pDrawSystem, pCommandList, dagValue.data);
         auto pDagValue = DagValue< std::shared_ptr< Geometry > >::Factory(pGeometry);
         auto pNode = DagNodeValue::Factory( pDagValue );
         applicationDagCollection.AddDagNode( dagValue.name, pNode);
      }
      break;
   case DagValueType::Shader:
      {
         auto pShader = JSONShader::Factory(pDrawSystem, pCommandList, pFileCache, dagValue.data);
         auto pDagValue = DagValue< std::shared_ptr< Shader > >::Factory(pShader);
         auto pNode = DagNodeValue::Factory( pDagValue );
         applicationDagCollection.AddDagNode( dagValue.name, pNode);
      }
      break;
   }
   return;
}

static void DealDagCalculate(ApplicationDagCollection& applicationDagCollection, const JSONDagCalculate& dagCalculate)
{
   auto pCallback = ApplicationDagCalculate::FindFunction( dagCalculate.function);
   auto pDagCalculate = std::make_shared< DagNodeCalculate >(pCallback);
   for (const auto& item : dagCalculate.stackInput)
   {
      if (false == item.is_string())
      {
         continue;
      }
      const auto name = item.get< std::string >();
      auto pNode = applicationDagCollection.GetDagNode(name);
      applicationDagCollection.LinkPush(pNode, pDagCalculate.get());
   }
   int trace = 0;
   for (const auto& item : dagCalculate.orderedInput)
   {
      iDagNode* pNode = nullptr;
      if (item.is_string())
      {
         const auto name = item.get< std::string >();
         pNode = applicationDagCollection.GetDagNode(name);
      }
      applicationDagCollection.LinkIndex(trace, pNode, pDagCalculate.get());
      trace += 1;
   }

   applicationDagCollection.AddDagNode(dagCalculate.name, pDagCalculate);
}

static void DealJson(
   DrawSystem* const pDrawSystem, 
   CustomCommandList* const pCommandList,
   FileCache* const pFileCache,
   ApplicationDagCollection& applicationDagCollection, 
   const nlohmann::json& jsonParent
   )
{
   //dagValues
   if (jsonParent.contains("dagValues"))
   {
      const auto& json = jsonParent["dagValues"];
      for (const auto& item : json)
      {
         const auto dagValue = item.get< JSONDagValue >();
         DealDagValue(pDrawSystem, pCommandList, pFileCache, applicationDagCollection, dagValue);
      }
   }

   //dagCalculate
   if (jsonParent.contains("dagCalculate"))
   {
      const auto& json = jsonParent["dagCalculate"];
      for (const auto& item : json)
      {
         const auto dagCalculate = item.get< JSONDagCalculate >();
         DealDagCalculate(applicationDagCollection, dagCalculate);
      }
   }
}

std::unique_ptr<ApplicationDagCollection> JSONApplicationDagCollection::Factory(
   DrawSystem* const pDrawSystem,
   CustomCommandList* const pCommandList,
   FileCache* const pFileCache,
   const nlohmann::json& jsonParent,
   const int width, 
   const int height
   )
{
   auto pResult = std::make_unique<ApplicationDagCollection>(width, height);

   if (jsonParent.contains("DagFiles"))
   {
      const auto& json = jsonParent["DagFiles"];
      for (const auto& iter : json)
      {
         if (false == iter.is_string())
         {
            continue;
         }
         const std::wstring path = Utf8::Utf8ToUtf16( iter.get< std::string >() );
         auto fileJson = nlohmann::json::parse( FileCache::RawLoadFileString(path));
         DealJson(pDrawSystem, pCommandList, pFileCache, *pResult, fileJson);
      }
   }

   return pResult;
}


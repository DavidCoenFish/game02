#include "CommonPCH.h"
#include "Common/JSON/JSONDagCollection.h"
#include "Common/DAG/iDagNode.h"
#include "Common/DAG/DagCollection.h"
#include "Common/Log/Log.h"


std::shared_ptr< DagCollection > JSONDagCollection::Factory(
   const std::string& fileName,
   const std::function<const std::string(const std::string& fileName)>& dealFileCallback,
   const std::vector< std::pair< std::string, std::shared_ptr< iDagNode > > >& inbuiltDagValues,
   const std::map<std::string, ValueFactory>& valueFactoryMap,
   const std::map<std::string, ValueFactory>& calculateFactoryMap
   )
{
   std::shared_ptr< DagCollection > pResult = std::make_shared< DagCollection >();
   for (const auto& item : inbuiltDagValues)
   {
      pResult->AddDagNode(item.first, item.second);
   }
   AppendCollection(pResult, fileName, dealFileCallback, valueFactoryMap, calculateFactoryMap);
   return pResult;
}

void JSONDagCollection::AppendCollection(
   const std::shared_ptr< DagCollection >& pDagCollection,
   const std::string& fileName,
   const std::function<const std::string(const std::string& fileName)>& dealFileCallback,
   const std::map<std::string, ValueFactory>& valueFactoryMap,
   const std::map<std::string, ValueFactory>& calculateFactoryMap
   )
{
   const auto data = dealFileCallback(fileName);
   auto json = nlohmann::json::parse( data );
   JSONDagCollection jsonDagCollection;
   json.get_to(jsonDagCollection);

   for (const auto& item : jsonDagCollection.valueArray)
   {
      auto found = valueFactoryMap.find(item.type);
      if (valueFactoryMap.end() == found)
      {
         LOG_MESSAGE_ERROR("Failed to find node value type:%s", item.type.c_str());
         continue;
      }
      auto pNode = found->second(item.data);
      pDagCollection->AddDagNode(item.name, pNode);
   }

   for (const auto& item : jsonDagCollection.calculateArray)
   {
      auto found = calculateFactoryMap.find(item.type);
      if (calculateFactoryMap.end() == found)
      {
         LOG_MESSAGE_ERROR("Failed to find node calculate type:%s", item.type.c_str());
         continue;
      }
      auto pNode = found->second(item.data);
      pDagCollection->AddDagNode(item.name, pNode);

      for (const auto& link : item.stackInput)
      {
         auto* pLinkInput = pDagCollection->GetDagNode(link);
         if (nullptr == pLinkInput)
         {
            LOG_MESSAGE_ERROR("Failed to link nodes:%s %s", item.name.c_str(), link.c_str());
            continue;
         }
         pDagCollection->LinkPush(pLinkInput, pNode.get());
      }

      for (int index = 0; index < (int)item.orderedInput.size(); ++index)
      {
         const auto name = item.orderedInput[index];
         auto* pLinkInput = pDagCollection->GetDagNode(name);
         if (nullptr == pLinkInput)
         {
            if (false == name.empty())
            {
               LOG_MESSAGE_ERROR("Failed to link index nodes:%s %d %s", item.name.c_str(), index, name.c_str());
            }
            //null indexed input is ok....
            continue;
         }
         pDagCollection->LinkIndex(index, pLinkInput, pNode.get());
      }
   }

   for (const auto& item : jsonDagCollection.fileArray)
   {
      AppendCollection(
         pDagCollection, 
         item,
         dealFileCallback,
         valueFactoryMap,
         calculateFactoryMap
         );
   }
}

#include "CommonPCH.h"
#include "Common/JSON/JSONDagCollection.h"
#include "Common/DAG/iDagNode.h"
#include "Common/DAG/DagCollection.h"
#include "Common/Log/Log.h"

//std::map<std::string, std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)>>& GetValueFactoryMap()
//{
//   static std::map<std::string, std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)>> s_map;
//   return s_map;
//}
//
//void JSONDagCollection::RegisterValue(const std::string& type, const std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)>& factory)
//{
//   auto& map = GetValueFactoryMap();
//   map[type] = factory;
//}
//
//std::map<std::string, std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)>>& GetCalculateFactoryMap()
//{
//   static std::map<std::string, std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)>> s_map;
//   return s_map;
//}
//
//void JSONDagCollection::RegisterCalculate(const std::string& function, const std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)>& factory)
//{
//   auto& map = GetCalculateFactoryMap();
//   map[function] = factory;
//}
//
std::shared_ptr< DagCollection > JSONDagCollection::Factory(
   const JSONDagCollection& jsonDagCollection,
   const std::vector< std::pair< std::string, std::shared_ptr< iDagNode > > >& inbuiltDagValues,
   const std::map<std::string, ValueFactory>& valueFactoryMap,
   const std::map<std::string, ValueFactory>& calculateFactoryMap
   )
{
   std::shared_ptr< DagCollection > pResult = std::make_shared< DagCollection >();
   for (const auto& item : jsonDagCollection.valueArray)
   {
      auto found = valueFactoryMap.find(item.type);
      if (valueFactoryMap.end() == found)
      {
         LOG_MESSAGE_ERROR("Failed to find node value type:%s", item.type.c_str());
         continue;
      }
      auto pNode = found->second(item.data);
      pResult->AddDagNode(item.name, pNode);
   }

   for (const auto& item : inbuiltDagValues)
   {
      pResult->AddDagNode(item.first, item.second);
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
      pResult->AddDagNode(item.name, pNode);

      for (const auto& link : item.stackInput)
      {
         auto* pLinkInput = pResult->GetDagNode(link);
         if (nullptr == pLinkInput)
         {
            LOG_MESSAGE_ERROR("Failed to link nodes:%s %s", item.name.c_str(), link.c_str());
            continue;
         }
         pResult->LinkPush(pLinkInput, pNode.get());
      }

      for (int index = 0; index < (int)item.orderedInput.size(); ++index)
      {
         const auto name = item.orderedInput[index];
         auto* pLinkInput = pResult->GetDagNode(name);
         if (nullptr == pLinkInput)
         {
            if (false == name.empty())
            {
               LOG_MESSAGE_ERROR("Failed to link index nodes:%s %d %s", item.name.c_str(), index, name.c_str());
            }
            //null indexed input is ok....
            continue;
         }
         pResult->LinkIndex(index, pLinkInput, pNode.get());
      }
   }

   return pResult;
}

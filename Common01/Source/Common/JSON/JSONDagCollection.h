#pragma once

#include "json/json.hpp"
class iDagNode;
class DagCollection;

struct JSONDagValue
{
   std::string name;
   std::string type;
   nlohmann::json data;

};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONDagValue, 
   name,
   type,
   data
   );

//struct JSONDagOrderedInput
//{
//   std::string input;
//   int inputIndex;
//};
//
//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
//   JSONDagOrderedInput, 
//   input,
//   inputIndex
//   );

struct JSONDagCalculate
{
   std::string name;
   std::string function;
   std::vector< std::string > stackInput;
   std::vector< std::string > orderedInput;
   
   //std::vector< JSONDagOrderedInput > orderedInputArray;
   nlohmann::json data;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONDagCalculate, 
   name,
   function,
   stackInput,
   orderedInput,
   data
   );


struct JSONDagCollection
{
   std::vector<JSONDagValue> valueArray;
   std::vector<JSONDagCalculate> calculateArray;

   static std::shared_ptr< DagCollection > Factory(
      const JSONDagCollection& jsonDagCollection,
      const std::map<std::string, std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)>>& valueMap,
      const std::map<std::string, std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)>>& calculateMap
      );
   //static void RegisterValue(const std::string& type, const std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)>& factory);
   //static void RegisterCalculate(const std::string& function, const std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)>& factory);
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONDagCollection, 
   valueArray,
   calculateArray
   );

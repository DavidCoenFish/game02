#pragma once

class iDagNode;
class iDagValue;

namespace ApplicationDagCalculate
{
   typedef std::function< std::shared_ptr< iDagValue >(const std::vector< iDagNode* >&, const std::vector< iDagNode* >&, const std::shared_ptr< iDagValue >&) > CalculateFunction;
   
   void AddFunction(const std::string& key, const CalculateFunction& calculateFunction);
   const CalculateFunction FindFunction(const std::string& key);

   void SetDefaultFunctions();
};

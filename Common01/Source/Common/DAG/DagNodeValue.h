#pragma once

#include "common/dag/idagnode.h"

//TYPE could be a std::shared_ptr< whatever >?

class iDagValue;

class DagNodeValue : public iDagNode
{
public:
   static std::shared_ptr< iDagNode > Factory( const std::shared_ptr< iDagValue >& pValue, const bool bMarkDirtyOnSet = true );

   DagNodeValue( const std::shared_ptr< iDagValue >& pValue, const bool bMarkDirtyOnSet = true );
   ~DagNodeValue();

private:
   virtual void SetValue( const std::shared_ptr< iDagValue >& pValue ) override;
   virtual std::shared_ptr< iDagValue >& GetValue( void ) override;
   virtual void OnMarkDirty() override;
   virtual void StackInputPush(iDagNode* const pNode)  override;
   virtual void OrderedInputSet(const int index, iDagNode* const pNodeOrNullptr)  override;
   virtual void StackInputRemove(iDagNode* const pNode)  override;
   virtual void RemoveAllInput(void) override;
   virtual void SetOutput(iDagNode* const pNode) override;
   virtual void RemoveOutput(iDagNode* const pNode) override;

private:
   const bool m_bMarkDirtyOnSet;
   std::vector< iDagNode* > m_arrayOutput;
   std::shared_ptr< iDagValue > m_pValue;
};


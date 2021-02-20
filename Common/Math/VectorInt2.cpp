#include "CommonPCH.h"

#include "Common/Math/VectorInt2.h"
VectorInt2::VectorInt2(const int x, const int y)
   : m_x(x)
   , m_y(y)
{
   return;
}

int& VectorInt2::operator[](const int index)
{
   switch(index)
   {
   case 0:
      return m_x;
   case 1:
      return m_y;
   default:
      break;
   }
   static int s_dummy = 0;
   return s_dummy;
}

const int VectorInt2::operator[](const int index)const
{
   switch(index)
   {
   case 0:
      return m_x;
   case 1:
      return m_y;
   default:
      break;
   }
   return 0;
}

const bool operator==(const VectorInt2& lhs, const VectorInt2& rhs)
{
   if (lhs[0] != rhs[0])
   {
      return false;
   }
   if (lhs[1] != rhs[1])
   {
      return false;
   }
   return true;
}


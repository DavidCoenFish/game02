#pragma once

class VectorInt2
{
public:
   explicit VectorInt2(const int x = 0, const int y = 0);

   int& operator[](const int index);
   const int operator[](const int index)const;

private:
   int m_x;
   int m_y;
};

const bool operator==(const VectorInt2& lhs, const VectorInt2& rhs);


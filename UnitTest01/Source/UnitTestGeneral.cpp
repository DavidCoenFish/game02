#include "UnitTestPCH.h"

#include "Common/Util/Utf8.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

namespace UnitTestGeneral
{
   TEST_CLASS(LanguageFeatures)
   {
   public:
 
      TEST_METHOD(VectorConstructionLiteral)
      {
         std::vector< int > data({0,1,2,3,4});
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual<int>(5, data.size() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(0, data[0] );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(1, data[1] );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(2, data[2] );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(3, data[3] );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(4, data[4] );
      }

      TEST_METHOD(VectorConstructionLiteralPointer)
      {
         std::vector< void* > data({nullptr,nullptr,nullptr,nullptr});
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual<int>(4, data.size() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual<void*>(nullptr, data[0] );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual<void*>(nullptr, data[1] );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual<void*>(nullptr, data[2] );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual<void*>(nullptr, data[3] );
      }

   };
}

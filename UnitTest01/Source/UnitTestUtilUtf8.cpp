#include "UnitTestPCH.h"

#include "Common/Util/Utf8.h"

//using namespace Microsoft::VisualStudio::CppUnitTestFramework;

//Assert::IsNotNull( pResultType );
//Assert::AreEqual(9, pResultType->Get<int>() );


namespace UtilUtf8Unittest
{
   TEST_CLASS(Basic)
   {

   private:
      void TestRoundTrip(const std::string& input)
      {
         auto temp0 = Utf8::Utf8ToUtf16(input);
         auto temp1 = Utf8::Utf16ToUtf8(temp0);
         auto temp2 = Utf8::Utf8ToUtf16(temp1);
         auto temp3 = Utf8::Utf16ToUtf8(temp2);
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(0, strcmp(input.c_str(), temp3.c_str()));
      }

   public:
 
      TEST_METHOD(RoundTrip0)
      {
         TestRoundTrip("abcd");
         TestRoundTrip("abcd\xE0\xB8\x8D");
      }

   };
}

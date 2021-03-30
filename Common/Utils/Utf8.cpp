#include "CommonPCH.h"

#include "Common/Utils/Utf8.h"

//https://stackoverflow.com/questions/2573834/c-convert-string-or-char-to-wstring-or-wchar-t
//https://stackoverflow.com/questions/6693010/how-do-i-use-multibytetowidechar
const std::wstring Utf8::Utf8ToUtf16( const std::string& in_utf8 )
{
#if defined(USE_CODECVT)
   std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
   //std::string narrow = converter.to_bytes(wide_utf16_source_string);
   const std::wstring wide = converter.from_bytes(in_utf8);
   return wide;
#else
   if (in_utf8.empty())
   {
      return std::wstring();
   }
   const size_t num_chars = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, in_utf8.c_str(), (int)in_utf8.length(), NULL, 0);
   std::wstring wstrTo;
   if (num_chars)
   {
      wstrTo.resize(num_chars);
      if (MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, in_utf8.c_str(), (int)in_utf8.length(), &wstrTo[0], (int)num_chars))
      {
         return wstrTo;
      }
   }
   return std::wstring();
#endif
}


const std::string Utf8::Utf16ToUtf8( const std::wstring& in_utf16 )
{
   std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>,wchar_t> convert; 
   std::string dest = convert.to_bytes(in_utf16);   
   return dest;
}

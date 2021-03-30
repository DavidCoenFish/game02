#include "CommonPCH.h"

#include "Common/Application/FileCache.h"
#include "Common/Application/ReadData.h"
#include "Common/Application/Log.h"
#include "Common/Utils/Utf8.h"

FileCache::FileCache()
{
   return;
}

FileCache::~FileCache()
{
   return;
}

std::shared_ptr< std::vector<uint8_t> > FileCache::RequestFile(const std::wstring& path)
{
   const auto found = m_fileCache.find( path );
   if (found != m_fileCache.end())
   {
      return found->second;
   }

   std::vector<uint8_t> data = DX::ReadData( path.c_str() );
   if (0 < data.size())
   {
      auto pData = std::make_shared< std::vector<uint8_t> >( data );
      m_fileCache[ path ] = pData;
      return pData;
   }
   return nullptr;
}

//just load a file without cache
std::vector<uint8_t> FileCache::RawLoadFile(const std::wstring& path)
{
   LOG_MESSAGE("FileCache::RawLoadFile %s\n", Utf8::Utf16ToUtf8(path).c_str());
   std::vector<uint8_t> data = DX::ReadData( path.c_str() );
   return data;
}

std::string FileCache::RawLoadFileString(const std::wstring& path)
{
   LOG_MESSAGE("FileCache::RawLoadFileString %s\n", Utf8::Utf16ToUtf8(path).c_str());
   std::vector<uint8_t> data = DX::ReadData( path.c_str() );
   data.push_back(0);
   return std::string((const char* const)data.data());
}


#include "CommonPCH.h"

#include "Common/FileSystem/FileSystem.h"
#include "Common/FileSystem/IReadOverlay.h"
#include "Common/FileSystem/IWriteOverlay.h"
#include "Common/Util/Utf8.h"

static const std::string DataToString(const std::vector<uint8_t>& data)
{
#if 0
   data.push_back(0);
   return std::string((const char* const)data.data());
#else
   static_assert(sizeof(char) == sizeof(uint8_t));
   const std::string result((char*)(data.data()), data.size()); 
   return result;
#endif
}

static const std::vector<uint8_t> StringToData(const std::string& string)
{
   static_assert(sizeof(char) == sizeof(uint8_t));
   //const std::vector<uint8_t> result((uint8_t*)string.data(), string.size());
   const std::vector<uint8_t> result(string.begin(), string.end());

   return result;
}

#define MAX_PATH_LENGTH 1024 
const std::filesystem::path FileSystem::GetModualDir(HINSTANCE hInstance)
{
   wchar_t data[MAX_PATH_LENGTH];
   const auto length = GetModuleFileNameW(hInstance, &data[0], MAX_PATH_LENGTH);
   if (length == MAX_PATH_LENGTH)
   {
      return std::string();
   }
   std::string exePath = Utf8::Utf16ToUtf8(data);
   std::filesystem::path path(exePath);
   path.remove_filename();
   return path;
}

const std::filesystem::path FileSystem::GetTempPath()
{
   return std::filesystem::temp_directory_path();
}

FileSystem::FileSystem()
   : m_bArrayOverlayReadOrderDirty(false)
{
   //nop
}

FileSystem::~FileSystem()
{
   //nop
}

void FileSystem::AddReadOverlay( const std::shared_ptr< IReadOverlay >& pOverlay )
{
   m_arrayOverlayRead.push_back(pOverlay);
   m_bArrayOverlayReadOrderDirty = true;
}

void FileSystem::RemoveReadOverlay( const std::shared_ptr< IReadOverlay >& pOverlay )
{
   auto result = std::remove(m_arrayOverlayRead.begin(), m_arrayOverlayRead.end(), pOverlay);
   result;
}

void FileSystem::AddWriteOverlay( const std::shared_ptr< IWriteOverlay >& pOverlay )
{
   m_arrayOverlayWrite.push_back(pOverlay);
}

void FileSystem::RemoveWriteOverlay( const std::shared_ptr< IWriteOverlay >& pOverlay )
{
   auto result = std::remove(m_arrayOverlayWrite.begin(), m_arrayOverlayWrite.end(), pOverlay);
   result;
}

std::shared_ptr< std::vector<uint8_t> > FileSystem::GetFileData(const std::filesystem::path& path, const bool bCacheFile)
{
   bCacheFile;
   SortReadOverlayArray();
   for( const auto& item : m_arrayOverlayRead)
   {
      auto pResult = item->GetFileData(path);
      if (nullptr != pResult)
      {
         return pResult;
      }
   }
   return std::shared_ptr< std::vector<uint8_t> >();
}

std::shared_ptr< std::string > FileSystem::GetFileString(const std::filesystem::path& path, const bool bCacheFile)
{
   auto pData = GetFileData(path, bCacheFile);
   if (nullptr != pData)
   {
      auto pResult = std::make_shared< std::string >( DataToString( *pData ) );
      return pResult;
   }
   return nullptr;
}

const int FileSystem::SaveFileData(const int filter, const std::filesystem::path& path, const std::vector<uint8_t>& data)
{
   int result = 0;
   for( const auto& item : m_arrayOverlayWrite)
   {
      if (0 == (filter & item->GetMask()))
      {
         continue;
      }
      if (true == item->SaveFileData(path, data))
      {
         result |= item->GetMask();
      }
   }
   return result;
}

const int FileSystem::SaveFileString(const int filter, const std::filesystem::path& path, const std::string& data)
{
   const auto arrayData = StringToData(data);
   const int result = SaveFileData(filter, path, arrayData);
   return result;
}

void FileSystem::SortReadOverlayArray()
{
   if (false == m_bArrayOverlayReadOrderDirty)
   {
      return;
   }
   m_bArrayOverlayReadOrderDirty = true;
   std::sort(m_arrayOverlayRead.begin(), m_arrayOverlayRead.end(), [](const std::shared_ptr< IReadOverlay >& lhs, const std::shared_ptr< IReadOverlay >& rhs)
      {
         return (lhs->GetPriority() < rhs->GetPriority());
      }
   );
   return;
}

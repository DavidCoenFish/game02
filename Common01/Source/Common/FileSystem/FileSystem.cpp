#include "CommonPCH.h"

#include "Common/FileSystem/FileSystem.h"
#include "Common/FileSystem/IReadOverlay.h"
#include "Common/FileSystem/IWriteOverlay.h"
#include "Common/Util/Utf8.h"

static std::vector< std::shared_ptr< IReadOverlay > >& GetArrayOverlayRead()
{
   static std::vector< std::shared_ptr< IReadOverlay > > sArrayOverlayRead;
   std::sort(sArrayOverlayRead.begin(), sArrayOverlayRead.end(), [](const std::shared_ptr< IReadOverlay >& lhs, const std::shared_ptr< IReadOverlay >& rhs)
      {
         return (rhs->GetPriority() < lhs->GetPriority());
      }
   );
   return sArrayOverlayRead;
}

static std::vector< std::shared_ptr< IWriteOverlay > >& GetArrayOverlayWrite()
{
   static std::vector< std::shared_ptr< IWriteOverlay > > sArrayOverlayWrite;
   return sArrayOverlayWrite;
}

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

const std::filesystem::path FileSystem::GetTempDir()
{
   return std::filesystem::temp_directory_path();
}

void FileSystem::AddReadOverlay( const std::shared_ptr< IReadOverlay >& pOverlay )
{
   GetArrayOverlayRead().push_back(pOverlay);
}

void FileSystem::RemoveReadOverlay( const std::shared_ptr< IReadOverlay >& pOverlay )
{
   auto& arrayOverlayRead = GetArrayOverlayRead();
   auto result = std::remove(arrayOverlayRead.begin(), arrayOverlayRead.end(), pOverlay);
   result;
}
void FileSystem::CleadReadOverlay()
{
   GetArrayOverlayRead().clear();
}

void FileSystem::AddWriteOverlay( const std::shared_ptr< IWriteOverlay >& pOverlay )
{
   GetArrayOverlayWrite().push_back(pOverlay);
}

void FileSystem::RemoveWriteOverlay( const std::shared_ptr< IWriteOverlay >& pOverlay )
{
   auto& arrayOverlayWrite = GetArrayOverlayWrite();
   auto result = std::remove(arrayOverlayWrite.begin(), arrayOverlayWrite.end(), pOverlay);
   result;
}
void FileSystem::CleadWriteOverlay()
{
   GetArrayOverlayWrite().clear();
}

std::shared_ptr< std::vector<uint8_t> > FileSystem::GetFileData(const std::filesystem::path& path, const bool bCacheFile)
{
   bCacheFile;
   auto& arrayOverlayRead = GetArrayOverlayRead();
   for( const auto& item : arrayOverlayRead)
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
   auto& arrayOverlayWrite = GetArrayOverlayWrite();
   for( const auto& item : arrayOverlayWrite)
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

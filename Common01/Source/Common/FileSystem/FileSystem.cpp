#include "CommonPCH.h"

#include "Common/FileSystem/FileSystem.h"
#include "Common/Util/Utf8.h"
#include "Common/Util/ThreadWrapper.h"

#define LOCAL_THREAD_COUNT 4

class FileSystemImplimentation
{
   FileSystemImplimentation();
   ~FileSystemImplimentation();

   void AddOverlay( const std::shared_ptr< IFileSystemOverlay >& pOverlay );
   void RemoveOverlay( const std::shared_ptr< IFileSystemOverlay >& pOverlay );
   void NotifyChangeOverlay( const std::shared_ptr< IFileSystemOverlay >& pOverlay );
   void LoadData( 
      const std::filesystem::path& path,
      const std::function< void(const std::shared_ptr< std::vector<uint8_t> >) >& callback
      );

   void SaveData(
      const std::shared_ptr< std::vector<uint8_t> >& data, 
      const std::filesystem::path& path, 
      const int filter,
      const std::function< void() >& callback
      );

   //DeleteFile is a define
   void DeleteDataFile(
      const std::filesystem::path& path, 
      const int filter,
      const std::function< void() >& callback
      );

   void FindFile(
      const std::filesystem::path& path, 
      const int filter,
      const std::function< void(std::shared_ptr<IFileSystemFoundFile>) >& callback
      );
   void FindFilePriorityExtention(
      const std::filesystem::path& path, 
      const std::vector< std::string >& extentionArrayHighestPriorityFirst,
      const int filter,
      const std::function< void(std::shared_ptr<IFileSystemFoundFile>) >& callback
      );
   void CollectFilesAtLocation(
      const std::filesystem::path& path, 
      const int filter,
      const std::function< void(std::shared_ptr<IFileSystemFoundFile>) >& callback
      );
 
   void RegisterWatchLocationChange(
      const std::filesystem::path& path, 
      const int filter,
      const std::function< void() >& callback
      );
   void ClearWatchLocationChange(
      const std::filesystem::path& path, 
      const int filter,
      const std::function< void() >& callback
      );
private:
   void DoWork();
   void SignalWorkToDo();

private:
   std::list< std::function<void()> > m_taskArray;
   std::mutex m_taskArrayMutex;

   std::vector< std::shared_ptr< IFileSystemOverlay > > m_overlayArray;
   std::mutex m_overlayArrayMutex;

   std::shared_ptr<ThreadWrapper<void()>> m_workerThread[LOCAL_THREAD_COUNT];

};

void FileSystemImplimentation::DoWork()
{
   std::function<void()> task;
   while (true)
   {
      {
         std::lock_guard< std::mutex > lock(m_taskArrayMutex);
         if (true == m_taskArray.empty())
         {
            return;
         }
         task = m_taskArray.front();
         m_taskArray.pop_front();
      }

      task();
   }
   //if there is any more on the array...? otherwise there is a risk of items never removed from list? use while loop instead
   //SignalWorkToDo();
}


void FileSystemImplimentation::SignalWorkToDo()
{
   for (int index = 0; index < LOCAL_THREAD_COUNT; ++index)
   {
      m_workerThread[index]->SignalWorkToDo();
   }
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

const int FileSystem::GetFilterAll()
{
   return 0xFFFFFFFF;
}

const int FileSystem::GetNewFilter()
{
   static int sFilterCount = 0;
   int value = 1 << sFilterCount;
   sFilterCount += 1;
   return value;
}

FileSystem::FileSystem()
{
   m_pImplimentation = std::make_unique<FileSystemImplimentation>();
}

FileSystem::~FileSystem()
{
   return;
}

void FileSystem::AddOverlay( const std::shared_ptr< IFileSystemOverlay >& pOverlay )
{
   if (nullptr != m_pImplimentation)
   {
      m_pImplimentation->AddOverlay(pOverlay);
   }
   return;
}

void FileSystem::RemoveOverlay( const std::shared_ptr< IFileSystemOverlay >& pOverlay )
{
   if (nullptr != m_pImplimentation)
   {
      m_pImplimentation->RemoveOverlay(pOverlay);
   }
   return;
}

void FileSystem::NotifyChangeOverlay( const std::shared_ptr< IFileSystemOverlay >& pOverlay )
{
   if (nullptr != m_pImplimentation)
   {
      m_pImplimentation->NotifyChangeOverlay(pOverlay);
   }
   return;
}

void FileSystem::LoadData( 
   const std::filesystem::path& path,
   const std::function< void(const std::shared_ptr< std::vector<uint8_t> >) >& callback
   )
{
   if (nullptr != m_pImplimentation)
   {
      m_pImplimentation->LoadData(path, callback);
   }
   return;
}

void FileSystem::SaveData(
   const std::shared_ptr< std::vector<uint8_t> >& data, 
   const std::filesystem::path& path, 
   const int filter,
   const std::function< void() >& callback
   )
{
   if (nullptr != m_pImplimentation)
   {
      m_pImplimentation->SaveData(data, path, filter, callback);
   }
   return;
}

void FileSystem::DeleteDataFile(
   const std::filesystem::path& path, 
   const int filter,
   const std::function< void() >& callback
   )
{
   if (nullptr != m_pImplimentation)
   {
      m_pImplimentation->DeleteDataFile(path, filter, callback);
   }
   return;
}

void FileSystem::FindFile(
   const std::filesystem::path& path, 
   const int filter,
   const std::function< void(std::shared_ptr<IFileSystemFoundFile>) >& callback
   )
{
   if (nullptr != m_pImplimentation)
   {
      m_pImplimentation->FindFile(path, filter, callback);
   }
   return;
}

void FileSystem::FindFilePriorityExtention(
   const std::filesystem::path& path, 
   const std::vector< std::string >& extentionArrayHighestPriorityFirst,
   const int filter,
   const std::function< void(std::shared_ptr<IFileSystemFoundFile>) >& callback
   )
{
   if (nullptr != m_pImplimentation)
   {
      m_pImplimentation->FindFilePriorityExtention(path, extentionArrayHighestPriorityFirst, filter, callback);
   }
   return;
}

void FileSystem::CollectFilesAtLocation(
   const std::filesystem::path& path, 
   const int filter,
   const std::function< void(std::shared_ptr<IFileSystemFoundFile>) >& callback
   )
{
   if (nullptr != m_pImplimentation)
   {
      m_pImplimentation->CollectFilesAtLocation(path, filter, callback);
   }
   return;
}

void FileSystem::RegisterWatchLocationChange(
   const std::filesystem::path& path, 
   const int filter,
   const std::function< void() >& callback
   )
{
   if (nullptr != m_pImplimentation)
   {
      m_pImplimentation->RegisterWatchLocationChange(path, filter, callback);
   }
   return;
}

void FileSystem::ClearWatchLocationChange(
   const std::filesystem::path& path, 
   const int filter,
   const std::function< void() >& callback
   )
{
   if (nullptr != m_pImplimentation)
   {
      m_pImplimentation->ClearWatchLocationChange(path, filter, callback);
   }
   return;
}




/*
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

std::mutex s_fileCacheMutex;
static std::map< std::filesystem::path, std::shared_ptr< std::vector<uint8_t> > >& GetFileCache()
{
   static std::map< std::filesystem::path, std::shared_ptr< std::vector<uint8_t> > > sFileCache;
   return sFileCache;
}

const std::string FileSystem::DataToString(const std::shared_ptr< std::vector<uint8_t> >& pData)
{
   if (nullptr != pData)
   {
      return DataToString(*pData);
   }
   return std::string();
}

const std::string FileSystem::DataToString(const std::vector<uint8_t>& data)
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

const std::vector<uint8_t> FileSystem::StringToData(const std::string& string)
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
   arrayOverlayRead.erase(std::remove(arrayOverlayRead.begin(), arrayOverlayRead.end(), pOverlay), arrayOverlayRead.end());
}
void FileSystem::ClearReadOverlay()
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
   arrayOverlayWrite.erase(std::remove(arrayOverlayWrite.begin(), arrayOverlayWrite.end(), pOverlay), arrayOverlayWrite.end());
}
void FileSystem::ClearWriteOverlay()
{
   GetArrayOverlayWrite().clear();
}

std::shared_ptr< std::vector<uint8_t> > FileSystem::ReadFileLoadData(const std::filesystem::path& path, const bool bCacheFile)
{
   if (true == path.empty())
   {
      return nullptr;
   }

   if (true == bCacheFile)
   {
      std::lock_guard< std::mutex > lock(s_fileCacheMutex);
      auto &map = GetFileCache();
      auto found = map.find(path);
      if (found != map.end())
      {
         return found->second;
      }
   }

   auto& arrayOverlayRead = GetArrayOverlayRead();
   for( const auto& item : arrayOverlayRead)
   {
      auto pResult = item->ReadFileLoadData(path);
      if (nullptr != pResult)
      {
         if (true == bCacheFile)
         {
            std::lock_guard< std::mutex > lock(s_fileCacheMutex);
            auto &map = GetFileCache();
            map[path] = pResult;
         }

         return pResult;
      }
   }
   return std::shared_ptr< std::vector<uint8_t> >();
}

const int FileSystem::WriteFileSaveData(const int filter, const std::filesystem::path& path, const std::vector<uint8_t>& data, const bool bAppend)
{
   int result = 0;
   auto& arrayOverlayWrite = GetArrayOverlayWrite();
   for( const auto& item : arrayOverlayWrite)
   {
      if (0 == (filter & item->GetMask()))
      {
         continue;
      }
      if (true == item->WriteFileSaveData(path, data, bAppend))
      {
         result |= item->GetMask();
      }
   }
   return result;
}

const int FileSystem::WriteFileDelete(const int filter, const std::filesystem::path& path)
{
   int result = 0;
   auto& arrayOverlayWrite = GetArrayOverlayWrite();
   for( const auto& item : arrayOverlayWrite)
   {
      if (0 == (filter & item->GetMask()))
      {
         continue;
      }
      if (true == item->WriteFileDelete(path))
      {
         result |= item->GetMask();
      }
   }
   return result;
}

void FileSystem::ReadClearFileCache()
{
   std::lock_guard< std::mutex > lock(s_fileCacheMutex);
   GetFileCache().clear();
}
*/
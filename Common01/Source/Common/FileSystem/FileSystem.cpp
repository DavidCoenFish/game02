#include "CommonPCH.h"

#include "Common/FileSystem/FileSystem.h"
#include "Common/FileSystem/IFileSystemOverlay.h"
#include "Common/Util/Utf8.h"
#include "Common/Util/WorkerCollection.h"

class FileSystemInternal
{
public:
   FileSystemInternal(const std::vector<std::shared_ptr< IFileSystemOverlay > >& arrayOverlay);
   ~FileSystemInternal();

   void AddJob( const std::function<void(void)>& job);

   void AsyncLoadData( 
      const std::filesystem::path& path,
      const std::function<void(const bool success, const int filter, const std::shared_ptr<std::vector<uint8_t>>& data)>& callback,
      const int filter
      );

   void AsyncSaveData(
      const std::shared_ptr< std::vector<uint8_t> >& data, 
      const std::filesystem::path& path, 
      const std::function< void(const bool success, const int filter) >& callback,
      const int filter
      );

   void AsyncDeleteFile(
      const std::filesystem::path& path, 
      const std::function< void(const bool success, const int filter) >& callback,
      const int filter
      );

private:
   void OnOverlayChangeFile(const std::filesystem::path& path, IFileSystemOverlay* const pOverlay);
   void OnOverlayChangeLocation(const std::filesystem::path& path, IFileSystemOverlay* const pOverlay);

private:
   //WorkerCollection< 4 > m_workerCollection;
   std::vector< std::shared_ptr< IFileSystemOverlay > > m_overlayArray;

};

FileSystemInternal::FileSystemInternal(const std::vector<std::shared_ptr< IFileSystemOverlay > >& arrayOverlay)
   : m_overlayArray(arrayOverlay)
{
   for (auto& iter : m_overlayArray)
   {
      iter->SetCallbackChangedFile([=](const std::filesystem::path& path, IFileSystemOverlay* const pOverlay){
            OnOverlayChangeFile(path, pOverlay);
         });
      iter->SetCallbackChangedLocation([=](const std::filesystem::path& path, IFileSystemOverlay* const pOverlay){
            OnOverlayChangeLocation(path, pOverlay);
         });
   }
}

FileSystemInternal::~FileSystemInternal()
{
}

void FileSystemInternal::AddJob( const std::function<void(void)>& job)
{
   job;
   //m_threadCollection.AddWork(job);
}

static IFileSystemOverlay* const FindOverlayFile(
   const std::vector<std::shared_ptr< IFileSystemOverlay > >& arrayOverlay,
   const std::filesystem::path& path,
   const int filter
   )
{
   IFileSystemOverlay* pOverlayFound = nullptr;
   for (auto& iter : arrayOverlay)
   {
      IFileSystemOverlay* pOverlay = iter.get();
      if (0 == (filter & pOverlay->GetFilter()))
      {
         continue;
      }
      if (false == pOverlay->ProvidesFile(path))
      {
         continue;
      }
      pOverlayFound = pOverlay;
   }

   return pOverlayFound;
}

void FileSystemInternal::AsyncLoadData( 
   const std::filesystem::path& path,
   const std::function<void(const bool success, const int filter, const std::shared_ptr<std::vector<uint8_t>>& data)>& callback,
   const int filter
   )
{
   IFileSystemOverlay* pOverlayFound = FindOverlayFile(m_overlayArray, path, filter);
   if (nullptr != pOverlayFound)
   {
      pOverlayFound->AsyncLoadData(
         path,
         callback
         );
   }
}

void FileSystemInternal::AsyncSaveData(
   const std::shared_ptr< std::vector<uint8_t> >& data, 
   const std::filesystem::path& path, 
   const std::function< void(const bool success, const int filter) >& callback,
   const int filter
   )
{
   IFileSystemOverlay* pOverlayFound = FindOverlayFile(m_overlayArray, path, filter);
   if (nullptr != pOverlayFound)
   {
      pOverlayFound->AsyncSaveData(
         path,
         data,
         callback
         );
   }
}

void FileSystemInternal::AsyncDeleteFile(
   const std::filesystem::path& path, 
   const std::function< void(const bool success, const int filter) >& callback,
   const int filter
   )
{
   IFileSystemOverlay* pOverlayFound = FindOverlayFile(m_overlayArray, path, filter);
   if (nullptr != pOverlayFound)
   {
      pOverlayFound->AsyncDeleteFile(
         path,
         callback
         );
   }
}


void FileSystemInternal::OnOverlayChangeFile(const std::filesystem::path& path, IFileSystemOverlay* const pOverlay)
{
   path;pOverlay;
}

void FileSystemInternal::OnOverlayChangeLocation(const std::filesystem::path& path, IFileSystemOverlay* const pOverlay)
{
   path;pOverlay;
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

std::shared_ptr< FileSystem > FileSystem::Factory(const std::vector<std::shared_ptr< IFileSystemOverlay > >& arrayOverlay)
{
   return std::make_shared< FileSystem >(arrayOverlay);
}

FileSystem::FileSystem(const std::vector<std::shared_ptr< IFileSystemOverlay > >& arrayOverlay)
{
   m_pInternal = std::make_unique<FileSystemInternal>(arrayOverlay);
}

FileSystem::~FileSystem()
{
   return;
}

//shortcut for FindFile -> AsyncLoadData, highest priority file that passes overlay filter
void FileSystem::AsyncLoadData( 
   const std::filesystem::path& path,
   const std::function<void(const bool success, const int filter, const std::shared_ptr<std::vector<uint8_t>>& data)>& callback,
   const int filter
   )
{
   auto pInternal = m_pInternal.get();
   pInternal->AddJob([=](){
      pInternal->AsyncLoadData( 
         path,
         callback,
         filter
         );
   });
}

//shortcut for FindFile -> AsyncSaveData, save the data on filtered overlays
void FileSystem::AsyncSaveData(
   const std::shared_ptr< std::vector<uint8_t> >& data, 
   const std::filesystem::path& path, 
   const std::function< void(const bool success, const int filter) >& callback,
   const int filter
   )
{
   auto pInternal = m_pInternal.get();
   pInternal->AddJob([=](){
      m_pInternal->AsyncSaveData(
         data,
         path,
         callback,
         filter
         );
   });
}

//delete the file on the filtered overlays
void FileSystem::AsyncDeleteFile(
   const std::filesystem::path& path, 
   const std::function< void(const bool success, const int filter) >& callback,
   const int filter
   )
{
   auto pInternal = m_pInternal.get();
   pInternal->AddJob([=](){
      m_pInternal->AsyncDeleteFile(
         path,
         callback,
         filter
         );
   });
}

//find the highest priority file that passes overlay filter, can then load, delete? the file
// to accomidate save, file may not exists?
const std::shared_ptr<FoundFile> FileSystem::FindFile(
   const std::filesystem::path& path, 
   const int filter
   )
{
   path;filter;
   return std::shared_ptr<FoundFile>();
}

const std::shared_ptr<FoundFile> FileSystem::FindFilePriorityExtention(
   const std::filesystem::path& path, 
   const std::vector< std::string >& extentionArrayHighestPriorityFirst,
   const int filter
   )
{
   path;extentionArrayHighestPriorityFirst;filter;
   return std::shared_ptr<FoundFile>();
}

const std::shared_ptr<FoundLocation> FileSystem::FindLocation(
   const std::filesystem::path& path, 
   const int filter
   )
{
   path;filter;
   return std::shared_ptr<FoundLocation>();
}


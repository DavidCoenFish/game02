#include "CommonPCH.h"

#include "Common/FileSystem/FileSystem.h"
#include "Common/FileSystem/IFileSystemProvider.h"
#include "Common/FileSystem/IFileSystemVisitorFound.h"
#include "Common/FileSystem/IFileSystemVisitorProvider.h"
#include "Common/FileSystem/FoundStaticFile.h"
#include "Common/FileSystem/FoundStaticFolder.h"
#include "Common/FileSystem/FoundDynamicFile.h"
#include "Common/FileSystem/FoundDynamicFolder.h"
#include "Common/Util/Utf8.h"
#include "Common/Worker/WorkerCollection.h"
#include "Common/Macro.h"
#include "Common/Worker/WorkerCount.h"

static const int GetNewFilter()
{
   static int sFilterCount = 0;
   int value = 1 << sFilterCount;
   DSC_ASSERT(sFilterCount < 31);
   sFilterCount += 1;
   return value;
}

class FileSystemInternal : public IFileSystemVisitorFound, public IFileSystemVisitorProvider
{
public:
   typedef std::function< void(void) > TVoidCallback;
   typedef uint32_t TFileHash;

   FileSystemInternal(const std::vector<std::shared_ptr< IFileSystemProvider > >& arrayOverlay);

   std::shared_ptr< FoundStaticFile > FindStaticFile(const std::filesystem::path& path, const int filter);
   std::shared_ptr< FoundStaticFile > FindStaticFilePriorityExtention(const std::filesystem::path& path, const std::vector<std::string>& priorityExtention, const int filter);
   std::shared_ptr< FoundStaticFolder > FindStaticFolder(const std::filesystem::path& path, const int filter);
   std::shared_ptr< FoundDynamicFile > FindDynamicFile(const std::filesystem::path& path, const int filter);
   std::shared_ptr< FoundDynamicFolder > FindDynamicFolder(const std::filesystem::path& path, const int filter);
   void AddCallbackAllProvidersReady(const int filter, const TVoidCallback& callback);

private:
   void GatherProviders(std::vector< IFileSystemProvider* >& arrayProvider, const int filter);

   //IFileSystemVisitorFound
   virtual void AddAsyncTask(const TVoidCallback& callback) override;
   virtual void AddFoundStaticFile(FoundStaticFile* const pFoundStaticFile) override;
   virtual void RemoveFoundStaticFile(FoundStaticFile* const pFoundStaticFile) override;

   //IFileSystemVisitorProvider
   virtual void OnReady(const IFileSystemProvider* pProvider) override;
   virtual void OnStaticFilesChange(IFileSystemProvider* const pProvider) override;

private:
   WorkerCollection<FILE_SYSTEM_WORKER_COUNT> m_workerCollection;

   //no mutex, constant after ctor
   std::vector<std::shared_ptr< IFileSystemProvider > > m_arrayOverlay;
   std::vector< std::pair< int, IFileSystemProvider* > > m_arrayFilterOverlay;

   //can change on multiple threads after ctor, under mutex
   std::mutex m_arrayReadyCallbackMutex;
   std::vector< std::pair< int, TVoidCallback > > m_arrayReadyCallback;
   int m_filterReady;

   /*
   do we need to make this one map per provider? to save itterating over so many on provider change?
   changing what files a provider has is always going to be expencive? minimise how many found file we keep active
   allow a path to load files skipping the creation of the found static file object?
   */
   //weak or raw, we are a cache, not a point of ownership.
   std::mutex m_setFoundStaticFileMutex;
   std::set<FoundStaticFile*> m_setFoundStaticFile;


};

FileSystemInternal::FileSystemInternal(const std::vector<std::shared_ptr< IFileSystemProvider > >& arrayOverlay)
   : m_arrayOverlay(arrayOverlay)
   , m_filterReady(0)
{
   for (const auto& iter : m_arrayOverlay)
   {
      const int filter = GetNewFilter();
      iter->SetFilter(filter);
      iter->SetFileSystemVisitorProvider(this);
      m_arrayFilterOverlay.push_back(std::pair< int, IFileSystemProvider* >(filter, iter.get()));
   }
   return;
}

std::shared_ptr< FoundStaticFile > FileSystemInternal::FindStaticFile(const std::filesystem::path& path, const int filter)
{
   std::vector< IFileSystemProvider* > arrayProvider;
   GatherProviders(arrayProvider, filter);
   if (true == arrayProvider.empty())
   {
      return nullptr;
   }

   auto pFoundFile = FoundStaticFile::Factory( path, filter, arrayProvider, this );
   return pFoundFile;
}

void FileSystemInternal::GatherProviders(std::vector< IFileSystemProvider* >& arrayProvider, const int filter)
{
   for (const auto& iter : m_arrayFilterOverlay)
   {
      if (0 == (iter.first & filter))
      {
         continue;
      }

      arrayProvider.push_back(iter.second);
   }
   return;
}

std::shared_ptr< FoundStaticFile > FileSystemInternal::FindStaticFilePriorityExtention(const std::filesystem::path& path, const std::vector<std::string>& priorityExtention, const int filter)
{
   std::vector< IFileSystemProvider* > arrayProvider;
   GatherProviders(arrayProvider, filter);
   if (true == arrayProvider.empty())
   {
      return nullptr;
   }

   auto pFoundFile = FoundStaticFile::Factory( path, filter, arrayProvider, this, priorityExtention );
   return pFoundFile;
}

std::shared_ptr< FoundStaticFolder > FileSystemInternal::FindStaticFolder(const std::filesystem::path& path, const int filter)
{
   path;filter;
   return nullptr;
}

std::shared_ptr< FoundDynamicFile > FileSystemInternal::FindDynamicFile(const std::filesystem::path& path, const int filter)
{
   path;filter;
   return nullptr;
}

std::shared_ptr< FoundDynamicFolder > FileSystemInternal::FindDynamicFolder(const std::filesystem::path& path, const int filter)
{
   path;filter;
   return nullptr;
}

void FileSystemInternal::AddCallbackAllProvidersReady(const int filter, const TVoidCallback& callback)
{
   std::lock_guard lock(m_arrayReadyCallbackMutex);
   m_arrayReadyCallback.push_back(std::pair< int, TVoidCallback >(filter, callback));
   return;
}

//IFileSystemProviderVisitor
void FileSystemInternal::OnReady(const IFileSystemProvider* pProvider)
{
   std::vector< TVoidCallback > pendingReadyCallback;
   {
      std::lock_guard lock(m_arrayReadyCallbackMutex);
      m_filterReady |= pProvider->GetFilter();

      auto copyArrayReadyCallback = m_arrayReadyCallback;
      m_arrayReadyCallback.clear();

      for (const auto& iter : m_arrayReadyCallback)
      {
         if (iter.first == (m_filterReady & iter.first))
         {
            pendingReadyCallback.push_back(iter.second);
         }
         else
         {
            m_arrayReadyCallback.push_back(iter);
         }
      }
   }

   for (const auto& iter : pendingReadyCallback)
   {
      iter();
   }
}

void FileSystemInternal::OnStaticFilesChange(IFileSystemProvider* const pProvider)
{
   const int filter = pProvider->GetFilter();
   {
      std::lock_guard lock(m_setFoundStaticFileMutex);
      for (auto& iter : m_setFoundStaticFile)
      {
         if (0 != (filter & iter->GetFilter()))
         {
            iter->OnProviderChange(pProvider);
         }
      }
   }
}

void FileSystemInternal::AddAsyncTask(const TVoidCallback& callback)
{
   m_workerCollection.AddWork(callback);
}

void FileSystemInternal::AddFoundStaticFile(FoundStaticFile* const pFoundStaticFile)
{
   std::lock_guard lock(m_setFoundStaticFileMutex);
   m_setFoundStaticFile.insert(pFoundStaticFile);
}

void FileSystemInternal::RemoveFoundStaticFile(FoundStaticFile* const pFoundStaticFile)
{
   std::lock_guard lock(m_setFoundStaticFileMutex);
   m_setFoundStaticFile.erase(pFoundStaticFile);
}

std::shared_ptr< FileSystem > FileSystem::Factory(const std::vector<std::shared_ptr< IFileSystemProvider > >& arrayOverlay)
{
   return std::make_shared< FileSystem >(arrayOverlay);
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

FileSystem::FileSystem(const std::vector<std::shared_ptr< IFileSystemProvider > >& arrayOverlay)
{
   m_pInternal = std::make_unique<FileSystemInternal>(arrayOverlay);
}

FileSystem::~FileSystem()
{
   m_pInternal = nullptr;
}

std::shared_ptr< FoundStaticFile > FileSystem::FindStaticFile(const std::filesystem::path& path, const int filter)
{
   return m_pInternal->FindStaticFile(path, filter);
}

std::shared_ptr< FoundStaticFile > FileSystem::FindStaticFilePriorityExtention(const std::filesystem::path& path, const std::vector<std::string>& priorityExtention, const int filter)
{
   return m_pInternal->FindStaticFilePriorityExtention(path, priorityExtention, filter);
}

std::shared_ptr< FoundStaticFolder > FileSystem::FindStaticFolder(const std::filesystem::path& path, const int filter)
{
   return m_pInternal->FindStaticFolder(path, filter);
}

std::shared_ptr< FoundDynamicFile > FileSystem::FindDynamicFile(const std::filesystem::path& path, const int filter)
{
   return m_pInternal->FindDynamicFile(path, filter);
}

std::shared_ptr< FoundDynamicFolder > FileSystem::FindDynamicFolder(const std::filesystem::path& path, const int filter)
{
   return m_pInternal->FindDynamicFolder(path, filter);
}

void FileSystem::AddCallbackAllProvidersReady(const int filter, const TVoidCallback& callback)
{
   return m_pInternal->AddCallbackAllProvidersReady(filter, callback);
}




#if 0

class FileSystemInternal
{
public:
   FileSystemInternal(const std::vector<std::shared_ptr< IFileSystemOverlay > >& arrayOverlay);
   ~FileSystemInternal();

   void AddJob( const TVoidCallback& job);

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

void FileSystemInternal::AddJob( const TVoidCallback& job)
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

#endif //0

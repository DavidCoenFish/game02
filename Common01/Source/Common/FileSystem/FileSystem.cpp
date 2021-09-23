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
   typedef std::function< void(const bool bError, const TFileData& pFileData) > TLoadCallback;
   typedef uint32_t TFileHash;

   FileSystemInternal(const std::vector<std::shared_ptr< IFileSystemProvider > >& arrayOverlay);

   std::shared_ptr< FoundStaticFile > FindStaticFile(const std::filesystem::path& path, const int filter);
   std::shared_ptr< FoundStaticFile > FindStaticFilePriorityExtention(const std::filesystem::path& path, const std::vector<std::string>& priorityExtention, const int filter);
   std::shared_ptr< FoundStaticFolder > FindStaticFolder(const std::filesystem::path& path, const int filter);
   std::shared_ptr< FoundDynamicFile > FindDynamicFile(const std::filesystem::path& path, const int filter);
   std::shared_ptr< FoundDynamicFolder > FindDynamicFolder(const std::filesystem::path& path, const int filter);
   void AddCallbackAllProvidersReady(const int filter, const TVoidCallback& callback);

   void AsyncStaticFileLoadBest(const std::filesystem::path& path, const TLoadCallback& callback, const int filter);
   void GatherStaticFolderContents(const std::filesystem::path& path, std::vector<std::filesystem::path>& childFiles, std::vector<std::filesystem::path>& childFolders, const int filter);
   void AsyncDynamicFileLoadBest(const std::filesystem::path& path, const TLoadCallback& callback, const int filter);
   void AsyncDynamicFileSaveAll(const std::filesystem::path& path, const TFileData& data, const int filter);
   void AsyncDynamicFileDeleteAll(const std::filesystem::path& path, const int filter);

private:
   void GatherProviders(std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProvider, const int filter);
   IFileSystemProvider* const GetBestProvider(const std::filesystem::path& path, const int filter);

   //IFileSystemVisitorFound
   virtual void AddAsyncTask(const TVoidCallback& callback) override;
   //virtual void AddFoundStaticFile(FoundStaticFile* const pFoundStaticFile) override;
   //virtual void RemoveFoundStaticFile(FoundStaticFile* const pFoundStaticFile) override;

   //IFileSystemVisitorProvider
   virtual void OnReady(const IFileSystemProvider* pProvider) override;
   //virtual void OnStaticFilesChange(IFileSystemProvider* const pProvider) override;

private:
   WorkerCollection<FILE_SYSTEM_WORKER_COUNT> m_workerCollection;

   //no mutex, constant after ctor
   std::vector< std::shared_ptr< IFileSystemProvider > > m_arrayProvider;
   typedef std::weak_ptr< IFileSystemProvider > TWeakProvider;
   std::vector< std::pair< int, TWeakProvider > > m_arrayFilterOverlay;
   std::vector< std::pair< int, IFileSystemProvider* > > m_arrayFilterOverlay2;

   //can change on multiple threads after ctor, under mutex
   std::mutex m_arrayReadyCallbackMutex;
   std::vector< std::pair< int, TVoidCallback > > m_arrayReadyCallback;
   int m_filterReady;

   /*
   do we need to make this one map per provider? to save itterating over so many on provider change?
   changing what files a provider has is always going to be expencive? minimise how many found file we keep active
   allow a path to load files skipping the creation of the found static file object?

   //weak or raw, we are a cache, not a point of ownership.
   std::mutex m_setFoundStaticFileMutex;
   std::set<FoundStaticFile*> m_setFoundStaticFile;
   */

};

FileSystemInternal::FileSystemInternal(const std::vector<std::shared_ptr< IFileSystemProvider > >& arrayOverlay)
   : m_arrayProvider(arrayOverlay)
   , m_filterReady(0)
{
   for (const auto& iter : m_arrayProvider)
   {
      const int filter = GetNewFilter();
      iter->SetFilter(filter);
      iter->SetFileSystemVisitorProvider(this);
      m_arrayFilterOverlay.push_back(std::pair< int, TWeakProvider >(filter, iter));
      m_arrayFilterOverlay2.push_back(std::pair< int, IFileSystemProvider* >(filter, iter.get()));
   }
   return;
}

std::shared_ptr< FoundStaticFile > FileSystemInternal::FindStaticFile(const std::filesystem::path& path, const int filter)
{
   std::vector< std::weak_ptr< IFileSystemProvider > > arrayProvider;
   GatherProviders(arrayProvider, filter);
   if (true == arrayProvider.empty())
   {
      return nullptr;
   }

   auto pFoundFile = FoundStaticFile::Factory( path, filter, arrayProvider, this );
   return pFoundFile;
}

void FileSystemInternal::GatherProviders(std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProvider, const int filter)
{
   for (const auto& iter : m_arrayFilterOverlay)
   {
      if (0 == (iter.first & filter))
      {
         continue;
      }

      arrayProvider.push_back(std::weak_ptr< IFileSystemProvider >(iter.second));
   }
   return;
}

std::shared_ptr< FoundStaticFile > FileSystemInternal::FindStaticFilePriorityExtention(const std::filesystem::path& path, const std::vector<std::string>& priorityExtention, const int filter)
{
   std::vector< std::weak_ptr< IFileSystemProvider > > arrayProvider;
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
   std::vector< std::weak_ptr< IFileSystemProvider > > arrayProvider;
   GatherProviders(arrayProvider, filter);
   if (true == arrayProvider.empty())
   {
      return nullptr;
   }

   auto pFoundFolder = FoundStaticFolder::Factory( path, filter, arrayProvider, this );
   return pFoundFolder;
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

IFileSystemProvider* const FileSystemInternal::GetBestProvider(const std::filesystem::path& path, const int filter)
{
   for (auto iter = m_arrayFilterOverlay2.rbegin(); 
           iter != m_arrayFilterOverlay2.rend(); ++iter )
   { 
      if (0 == (filter & iter->first))
      {
         continue;
      }
      IFileSystemProvider* pProvider = iter->second;
      if (nullptr == pProvider)
      {
         continue;
      }
      TFileHash hash = 0;
      if (pProvider->QueryStaticFile(hash, path))
      {
         return pProvider;
      }
   }

   return nullptr;
}

void FileSystemInternal::AsyncStaticFileLoadBest(const std::filesystem::path& path, const TLoadCallback& callback, const int filter)
{
   auto pProvider = GetBestProvider(path, filter);

   if (nullptr != pProvider)
   {
      AddAsyncTask([=](){
         pProvider->AsyncLoadStaticFile(callback, path);
      });
   }
   else
   {
      callback(false, nullptr);
   }
   return;
}

void FileSystemInternal::GatherStaticFolderContents(const std::filesystem::path& path, std::vector<std::filesystem::path>& childFiles, std::vector<std::filesystem::path>& childFolders, const int filter)
{
   for (const auto& iter : m_arrayFilterOverlay2)
   {
      if (0 == (filter & iter.first))
      {
         continue;
      }
      IFileSystemProvider* pProvider = iter.second;
      if (nullptr == pProvider)
      {
         continue;
      }
      pProvider->GatherStaticFolderContents(childFiles, childFolders, path);
   }
}

void FileSystemInternal::AsyncDynamicFileLoadBest(const std::filesystem::path& path, const TLoadCallback& callback, const int filter)
{
   auto pProvider = GetBestProvider(path, filter);

   if (nullptr != pProvider)
   {
      AddAsyncTask([=](){
         pProvider->AsyncLoadDynamicFile(callback, path);
      });
   }
   else
   {
      callback(false, nullptr);
   }
   return;
}

void FileSystemInternal::AsyncDynamicFileSaveAll(const std::filesystem::path& path, const TFileData& data, const int filter)
{
   for (const auto& iter : m_arrayFilterOverlay2)
   {
      if (0 == (filter & iter.first))
      {
         continue;
      }
      IFileSystemProvider* pProvider = iter.second;
      if (nullptr == pProvider)
      {
         continue;
      }
      AddAsyncTask([=](){
         pProvider->AsyncSaveDynamicFile(path, data);
      });
   }
}

void FileSystemInternal::AsyncDynamicFileDeleteAll(const std::filesystem::path& path, const int filter)
{
   for (const auto& iter : m_arrayFilterOverlay2)
   {
      if (0 == (filter & iter.first))
      {
         continue;
      }
      IFileSystemProvider* pProvider = iter.second;
      if (nullptr == pProvider)
      {
         continue;
      }
      AddAsyncTask([=](){
         pProvider->AsyncDeleteDynamicFile(path);
      });
   }
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

void FileSystemInternal::AddAsyncTask(const TVoidCallback& callback)
{
   m_workerCollection.AddWork(callback);
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

void FileSystem::AddCallbackAllProvidersReady(const TVoidCallback& callback, const int filter)
{
   return m_pInternal->AddCallbackAllProvidersReady(filter, callback);
}

void FileSystem::AsyncStaticFileLoadBest(const std::filesystem::path& path, const TLoadCallback& callback, const int filter)
{
   return m_pInternal->AsyncStaticFileLoadBest(path, callback, filter);
}

void FileSystem::GatherStaticFolderContents(const std::filesystem::path& path, std::vector<std::filesystem::path>& childFiles, std::vector<std::filesystem::path>& childFolders, const int filter)
{
   return m_pInternal->GatherStaticFolderContents(path, childFiles, childFolders, filter);
}

void FileSystem::AsyncDynamicFileLoadBest(const std::filesystem::path& path, const TLoadCallback& callback, const int filter)
{
   return m_pInternal->AsyncDynamicFileLoadBest(path, callback, filter);
}

void FileSystem::AsyncDynamicFileSaveAll(const std::filesystem::path& path, const TFileData& data, const int filter)
{
   return m_pInternal->AsyncDynamicFileSaveAll(path, data, filter);
}
void FileSystem::AsyncDynamicFileDeleteAll(const std::filesystem::path& path, const int filter)
{
   return m_pInternal->AsyncDynamicFileDeleteAll(path, filter);
}



#include "CommonPCH.h"

#include "Common/FileSystem/FoundStaticFile.h"
#include "Common/FileSystem/IFileSystemProvider.h"
#include "Common/FileSystem/IFileSystemVisitorFound.h"

std::shared_ptr< FoundStaticFile > FoundStaticFile::Factory(
   const std::filesystem::path& path, 
   const int filter,
   const std::vector<IFileSystemProvider*>& arrayProviders,
   IFileSystemVisitorFound* pFileSystem,
   const std::vector<std::string>& priorityExtention
   )
{
   return std::make_shared<FoundStaticFile>(
      path, 
      priorityExtention,
      filter,
      arrayProviders,
      pFileSystem
      );
}

FoundStaticFile::FoundStaticFile(
   const std::filesystem::path& path, 
   const std::vector<std::string>& priorityExtention,
   const int filter,
   const std::vector<IFileSystemProvider*>& arrayProviders,
   IFileSystemVisitorFound* pFileSystem
   )
   : m_path(path)
   , m_priorityExtention(priorityExtention)
   , m_filter(filter)
   , m_arrayProvider(arrayProviders)
   , m_pFileSystem(pFileSystem)
   , m_pBestProvider(nullptr)
   , m_bestHash(0)
   , m_bestFound(false)
{
   m_pFileSystem->AddFoundStaticFile(this);
   EvalueBest();
   return;
}

FoundStaticFile::~FoundStaticFile()
{
   m_pFileSystem->RemoveFoundStaticFile(this);
   return;
}

const int FoundStaticFile::GetFilter() const
{
   return m_filter;
}

void FoundStaticFile::OnProviderChange(IFileSystemProvider* const)
{
   if (false == EvalueBest())
   {
      return;
   }

   IFileSystemProvider* pBestProvider = nullptr;
   std::vector< TLoadCallback > arrayCallbackBestChange;
   {
      std::lock_guard lock(m_bestMutex);
      arrayCallbackBestChange = m_arrayCallbackBestChange;
      pBestProvider = m_pBestProvider;
   }
   if (nullptr != pBestProvider)
   {
      for (const auto& iter : arrayCallbackBestChange)
      {
         m_pFileSystem->AddAsyncTask([=](){
            pBestProvider->AsyncLoadStaticFile(iter);
         });
      }
   }
}

//when the function was called, the file existed
const bool FoundStaticFile::GetExist() const
{
   std::lock_guard lock(m_bestMutex);
   return m_bestFound;
}

//load the best file that passes filter, return false if file doesn't exist
void FoundStaticFile::AsyncLoadBest(const TLoadCallback& loadCallback)
{
   IFileSystemProvider* pBestProvider = nullptr;
   {
      std::lock_guard lock(m_bestMutex);
      pBestProvider = m_pBestProvider;
   }
   if (nullptr != pBestProvider)
   {
      m_pFileSystem->AddAsyncTask([=](){
         pBestProvider->AsyncLoadStaticFile(loadCallback);
      });
   }
}

//load each file that passes filter (ie, from possibly multiple providers)
//void AsyncLoadAll(const TLoadCallback& loadCallback);
   
// trigger callback if the best files has a diffierent hash, callback triggers as soon as run as you don't start with a file, thus it has changed?
void FoundStaticFile::AddCallbackChangeBest(const TLoadCallback& loadCallback)
{
   IFileSystemProvider* pBestProvider = nullptr;
   {
      std::lock_guard lock(m_bestMutex);
      m_arrayCallbackBestChange.push_back(loadCallback);
      pBestProvider = m_pBestProvider;
   }
   if (nullptr != pBestProvider)
   {
      m_pFileSystem->AddAsyncTask([=](){
         pBestProvider->AsyncLoadStaticFile(loadCallback);
      });
   }
   return;
}

//https://stackoverflow.com/questions/20833453/comparing-stdfunctions-for-equality
template<typename T, typename... U>
size_t getAddress(std::function<T(U...)> f) {
    typedef T(fnType)(U...);
    fnType ** fnPointer = f.template target<fnType*>();
    return (size_t) *fnPointer;
}

void FoundStaticFile::RemoveCallbackChangeBest(const TLoadCallback& loadCallback)
{
   std::lock_guard lock(m_bestMutex);
   const size_t lhs = getAddress(loadCallback);
   m_arrayCallbackBestChange.erase(
      std::remove_if(m_arrayCallbackBestChange.begin(), m_arrayCallbackBestChange.end(), [=](const TLoadCallback& rhs){
         return (lhs == getAddress(rhs));
      }), 
      m_arrayCallbackBestChange.end()
      );

   return;
}

const bool FoundStaticFile::EvalueBest()
{
   const TFileHash oldBestHash(m_bestFound);
   m_bestFound = false;
   m_bestHash = 0;
   m_pBestProvider = nullptr;

   std::lock_guard lock(m_bestMutex);
   for (const auto& iter: m_arrayProvider)
   {
      TFileHash hash = 0;
      if (iter->QueryStaticFile(hash, m_path))
      {
         m_bestFound = true;
         m_bestHash = hash;
         m_pBestProvider = iter;
      }
   }

   //want callback change to trigger when file is removed?
   //return ((true == m_bestFound) && (oldBestHash != m_bestHash));
   return (oldBestHash != m_bestHash);
}

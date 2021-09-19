#include "CommonPCH.h"

#include "Common/FileSystem/FoundStaticFolder.h"
#include "Common/FileSystem/IFileSystemProvider.h"
#include "Common/FileSystem/IFileSystemVisitorFound.h"

std::shared_ptr< FoundStaticFolder > FoundStaticFolder::Factory(
   const std::filesystem::path& path, 
   const int filter,
   const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
   IFileSystemVisitorFound* pFileSystem
   )
{
   return std::make_shared<FoundStaticFolder>(
      path, 
      filter,
      arrayProviders,
      pFileSystem
      );
}

FoundStaticFolder::FoundStaticFolder(
   const std::filesystem::path& path, 
   const int filter,
   const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
   IFileSystemVisitorFound* pFileSystem
   )
   : m_path(path)
   , m_filter(filter)
   , m_arrayProvider(arrayProviders)
   , m_pFileSystem(pFileSystem)
   , m_found(false)
{
   for (const auto& iter: m_arrayProvider)
   {
      auto pPointer = iter.lock();
      if (nullptr != pPointer)
      {
         pPointer->AddFoundStaticFolder(this);
         m_found |= pPointer->QueryStaticFolder(m_path);
      }
   }

   return;
}

FoundStaticFolder::~FoundStaticFolder()
{
   for (const auto& iter: m_arrayProvider)
   {
      auto pPointer = iter.lock();
      if (nullptr != pPointer)
      {
         pPointer->RemoveFoundStaticFolder(this);
      }
   }

   return;
}

//when the function was called, the file existed
const bool FoundStaticFolder::GetExist() const
{
   std::lock_guard lock(m_foundMutex);
   return m_found;
}

const int FoundStaticFolder::GetFilter() const
{
   return m_filter;
}

void FoundStaticFolder::OnProviderChange(IFileSystemProvider* const pProvider)
{
   bool found = false;
   for (const auto& iter: m_arrayProvider)
   {
      auto pPointer = iter.lock();
      if (nullptr != pPointer)
      {
         found |= pPointer->QueryStaticFolder(m_path);
      }
   }

   {
      std::lock_guard lock(m_foundMutex);
      found = false;
   }
}


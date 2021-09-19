#include "CommonPCH.h"

#include "Common/FileSystem/ProviderMemory.h"
#include "Common/FileSystem/ComponentFileMap.h"
#include "Common/FileSystem/ComponentFound.h"
#include "Common/FileSystem/IFileSystemVisitorProvider.h"

std::shared_ptr<ProviderMemory> ProviderMemory::Factory(
   const TStaticFileMap& staticFiles,
   const TDynamicFileMap& dynamicFiles
   )
{
   auto pComponentStaticFileMap = TComponentStaticFileMap::Factory(staticFiles);
   auto pComponentDynamicFileMap = TComponentDynamicFileMap::Factory(dynamicFiles);
   return std::make_shared<ProviderMemory>( pComponentStaticFileMap, pComponentDynamicFileMap );
}

ProviderMemory::ProviderMemory(
   const TPointerComponentStaticFileMap& pComponentStaticFileMap,
   const TPointerComponentDynamicFileMap& pComponentDynamicFileMap
   )
   : m_filter(0)
   , m_pVisitor(nullptr)
   , m_pComponentStaticFileMap(pComponentStaticFileMap)
   , m_pComponentDynamicFileMap(pComponentDynamicFileMap)
{
   m_pComponentFound = std::make_unique<ComponentFound>();
   return;
}

ProviderMemory::~ProviderMemory()
{
   m_pComponentFound = nullptr;
   return;
}

void ProviderMemory::SetFilter(const int filter)
{
   m_filter = filter;
}

const int ProviderMemory::GetFilter()const
{
   return m_filter;
}

void ProviderMemory::SetFileSystemVisitorProvider(IFileSystemVisitorProvider* const pVisitor)
{
   m_pVisitor = pVisitor;
   m_pVisitor->OnReady(this);
}

const bool ProviderMemory::QueryStaticFile(TFileHash& hashIfFound, const std::filesystem::path& path)
{
   TPointerStaticFileData pData;
   if (true == m_pComponentStaticFileMap->GetFile(path, pData))
   {
      hashIfFound = pData->m_fileHash;
      return true;
   }
   hashIfFound = 0;
   return false;
}

void ProviderMemory::AsyncLoadStaticFile(const TLoadCallback& loadCallback, const std::filesystem::path& path)
{
   TPointerStaticFileData pData;
   if (true == m_pComponentStaticFileMap->GetFile(path, pData))
   {
      loadCallback(pData->m_fileData);
   }
   return;
}

const bool ProviderMemory::QueryStaticFolder(const std::filesystem::path& path)
{
   return m_pComponentStaticFileMap->HasFolder(path);
}

const bool ProviderMemory::GatherStaticFolderContents(
   std::vector< std::filesystem::path >& childFiles,
   std::vector< std::filesystem::path >& childFolders,
   const std::filesystem::path& path
   )
{
   return m_pComponentStaticFileMap->GatherFolder(
      path,
      childFiles,
      childFolders
      );
}

void ProviderMemory::AddFoundStaticFile(FoundStaticFile* const pFoundStaticFile)
{
   m_pComponentFound->AddFoundStaticFile(pFoundStaticFile);
}

void ProviderMemory::RemoveFoundStaticFile(FoundStaticFile* const pFoundStaticFile) 
{
   m_pComponentFound->RemoveFoundStaticFile(pFoundStaticFile);
}

void ProviderMemory::AddFoundStaticFolder(FoundStaticFolder* const pFoundStaticFolder) 
{
   m_pComponentFound->AddFoundStaticFolder(pFoundStaticFolder);
}

void ProviderMemory::RemoveFoundStaticFolder(FoundStaticFolder* const pFoundStaticFolder) 
{
   m_pComponentFound->RemoveFoundStaticFolder(pFoundStaticFolder);
}

void ProviderMemory::AddFoundDynamicFile(FoundDynamicFile* const pFoundDynamicFile) 
{
   m_pComponentFound->AddFoundDynamicFile(pFoundDynamicFile);
}

void ProviderMemory::RemoveFoundDynamicFile(FoundDynamicFile* const pFoundDynamicFile) 
{
   m_pComponentFound->RemoveFoundDynamicFile(pFoundDynamicFile);
}

void ProviderMemory::AddFoundDynamicFolder(FoundDynamicFolder* const pFoundDynamicFolder) 
{
   m_pComponentFound->AddFoundDynamicFolder(pFoundDynamicFolder);
}

void ProviderMemory::RemoveFoundDynamicFolder(FoundDynamicFolder* const pFoundDynamicFolder) 
{
   m_pComponentFound->RemoveFoundDynamicFolder(pFoundDynamicFolder);
}



#include "CommonPCH.h"

#include "Common/FileSystem/ProviderMemory.h"
#include "Common/FileSystem/ComponentFileMap.h"
#include "Common/FileSystem/IFileSystemVisitorProvider.h"

std::shared_ptr<ProviderMemory> ProviderMemory::Factory(
   const TStaticFileMap& staticFiles,
   const TDynamicFileMap& dynamicFiles
   )
{
   auto pComponentFileMap = std::make_shared<TComponentFileMap>();
   for (const auto& iter : staticFiles )
   {
      auto pData = std::make_shared< TStaticFileData>();
      pData->m_fileHash = iter.second.second;
      pData->m_fileData = iter.second.first;
      pComponentFileMap->AddStaticFile(iter.first, pData);
   }
   for (const auto& iter : dynamicFiles )
   {
      auto pData = std::make_shared< TDynamicFileData >();
      pData->m_fileData = iter.second;
      pComponentFileMap->AddDynamicFile(iter.first, pData);
   }

   return std::make_shared<ProviderMemory>( pComponentFileMap );
}

ProviderMemory::ProviderMemory(
   const TPointerComponentFileMap& pComponentFileMap
   )
   : m_filter(0)
   , m_pVisitor(nullptr)
   , m_pComponentFileMap(pComponentFileMap)
{
   return;
}

ProviderMemory::~ProviderMemory()
{
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
   if (true == m_pComponentFileMap->GetStaticFile(path, pData))
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
   if (true == m_pComponentFileMap->GetStaticFile(path, pData))
   {
      loadCallback(pData->m_fileData);
   }
   return;
}

#include "CommonPCH.h"

#include "Common/FileSystem/OverlayMemory.h"

OverlayMemory::OverlayMemory(
   const int priority, 
   const int mask, 
   const std::map< std::filesystem::path, std::shared_ptr< std::vector<uint8_t>>>& memoryFiles
   )
: m_priority(priority)
, m_mask(mask)
, m_memoryFiles(memoryFiles)
{
   //nop
}

OverlayMemory::~OverlayMemory()
{
   //nop
}

std::shared_ptr< std::vector<uint8_t> > OverlayMemory::GetFileData(const std::filesystem::path& path)
{
   auto found = m_memoryFiles.find(path);
   if (found != m_memoryFiles.end())
   {
      return found->second;
   }
   return nullptr;
}

const bool OverlayMemory::SaveFileData(const std::filesystem::path& path, const std::vector<uint8_t>& data, const bool bAppend)
{
   if (true == bAppend)
   {
      auto found = m_memoryFiles.find(path);
      if (found != m_memoryFiles.end())
      {
         found->second->insert(found->second->end(), data.begin(), data.end());
         return true;
      }
   }

   m_memoryFiles[path] = std::make_shared< std::vector<uint8_t> >( data );
   return true;
}

//return true if delete worked
const bool OverlayMemory::DeleteSaveFile(const std::filesystem::path& path)
{
   auto found = m_memoryFiles.find(path);
   if (found != m_memoryFiles.end())
   {
      m_memoryFiles.erase(found);
      return true;
   }
   return false;
}

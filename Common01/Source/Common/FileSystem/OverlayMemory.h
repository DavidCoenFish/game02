#pragma once
#include "Common/FileSystem/IFileSystemOverlay.h"

class OverlayMemory : public IFileSystemOverlay
{
public:
   OverlayMemory(
      const int priority, 
      const int mask, 
      const std::map< std::filesystem::path, std::shared_ptr< std::vector<uint8_t>>>& memoryFiles = std::map< std::filesystem::path, std::shared_ptr< std::vector<uint8_t>>>()
      );
   ~OverlayMemory();

private:

private:
   std::map< std::filesystem::path, std::shared_ptr< std::vector<uint8_t>>> m_memoryFiles;

};

#pragma once
#include "Common/FileSystem/IFileSystemOverlay.h"

class ComponentFileMap;
class OverlayDisk : public IFileSystemOverlay
{
public:
   OverlayDisk(const int filter, const std::filesystem::path& basePath);
   ~OverlayDisk();

private:

private:
   int m_filter;
   std::filesystem::path m_basePath;
   std::unique_ptr< ComponentFileMap > m_componentFileMap;

};

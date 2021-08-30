#pragma once
#include "Common/FileSystem/IFileSystemOverlay.h"

class OverlayMods : public IFileSystemOverlay
{
public:
   OverlayMods(const int filter);
   ~OverlayMods();

   void AddMod(const std::filesystem::path& path);
   void RemoveMod(const std::filesystem::path& path);

private:

private:
   int m_filter;

};

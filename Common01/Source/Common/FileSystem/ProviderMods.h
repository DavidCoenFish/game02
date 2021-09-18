#pragma once
#include "Common/FileSystem/IFileSystemProvider.h"

class ProviderMods : public IFileSystemProvider
{
public:
   ProviderMods(const int filter);
   ~ProviderMods();

   //get told to do the startup work on some thread when things are ready, once done we tell IFileSystemVisitorProvider we are ready
   // have a dependency on base file system for the list of active mods, or is that external
   //void DoStartupWork();

   void AddMod(const std::filesystem::path& path);
   void RemoveMod(const std::filesystem::path& path);

private:

private:
   int m_filter;

};

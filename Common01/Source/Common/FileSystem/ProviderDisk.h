#pragma once
#include "Common/FileSystem/IFileSystemProvider.h"
#include "Common/FileSystem/ComponentFileMap.h"

class ProviderDisk : public IFileSystemProvider
{
public:
   typedef uint32_t TFileHash;

   ProviderDisk(const std::filesystem::path& basePath);
   ~ProviderDisk();

   //get told to do the startup work on some thread when things are ready, once done we tell IFileSystemVisitorProvider we are ready
   void DoStartupWork();

private:
   virtual void SetFilter(const int filter) override; //for FileSystem only?
   virtual const int GetFilter()const override;
   //onReady, onChangeStatic, onChangeDynamic? return a map of all the files/ locations, hash of static files... split dynamic & static
   virtual void SetFileSystemVisitorProvider(IFileSystemVisitorProvider* const pVisitor) override;

   virtual const bool QueryStaticFile(TFileHash& hashIfFound, const std::filesystem::path& path) override;

   //we don't care if the found hash changes between QueryStaticFile and AsyncLoadStaticFile calls, as we will have told the IFileSystem that we changed
   virtual void AsyncLoadStaticFile(const TLoadCallback& loadCallback, const std::filesystem::path& path) override;

private:
   std::filesystem::path m_basePath;
   std::unique_ptr< ComponentFileMap< TFileHash > > m_componentFileMap;
   int m_filter;
   IFileSystemVisitorProvider* m_pVisitorProvider;

};

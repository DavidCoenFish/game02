#pragma once
#include "Common/FileSystem/IFileSystemProvider.h"

class ProviderMemory : public IFileSystemProvider
{
public:
   typedef std::shared_ptr< std::vector< uint8_t > > TFileData;
   typedef uint32_t TFileHash;
   typedef std::pair< TFileData, TFileHash > TFile;
   typedef std::map< std::filesystem::path, TFile> TFileMap;

   typedef std::pair< std::vector<std::filesystem::path>, std::vector<std::filesystem::path> > TArrayFileArrayFolder;
   typedef std::map< std::filesystem::path, TArrayFileArrayFolder> TFolderMap;

   ProviderMemory(
      const TFileMap& staticFiles = TFileMap(),
      const TFileMap& dynamicFiles = TFileMap()
      );
   virtual ~ProviderMemory();

private:
   virtual void SetFilter(const int filter) override; //for FileSystem only?
   virtual const int GetFilter()const override;
   //onReady, onChangeStatic, onChangeDynamic? return a map of all the files/ locations, hash of static files... split dynamic & static
   virtual void SetFileSystemVisitorProvider(const IFileSystemVisitorProvider* pVisitor) override;

   virtual const bool QueryStaticFile(TFileHash& hashIfFound, const std::filesystem::path& path) override;

   //we don't care if the found hash changes between QueryStaticFile and AsyncLoadStaticFile calls, as we will have told the IFileSystemVisitorProvider that we changed
   virtual void AsyncLoadStaticFile(const TLoadCallback& loadCallback) override;

private:
   int m_filter;
   IFileSystemVisitorProvider* m_pVisitor;

   TFileMap m_staticFiles;
   TFolderMap m_staticFolders;
   TFileMap m_dynamicFiles;
   TFolderMap m_dynamicFolders;

};

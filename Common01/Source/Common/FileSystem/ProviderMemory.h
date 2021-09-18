#pragma once
#include "Common/FileSystem/IFileSystemProvider.h"
#include "Common/FileSystem/ComponentFileMap.h"

/*
this was intened for 
*/

//struct TComponentFileMapEmpty;
//template <typename TStaticFileData = TComponentFileMapEmpty, typename TDynamicFileData = TComponentFileMapEmpty>
//class ComponentFileMap;

class ProviderMemory : public IFileSystemProvider
{
public:
   typedef std::shared_ptr< std::vector< uint8_t > > TFileData;
   typedef uint32_t TFileHash;
   typedef std::pair< TFileData, TFileHash > TFile;
   typedef std::map< std::filesystem::path, TFile> TStaticFileMap;
   typedef std::map< std::filesystem::path, TFileData> TDynamicFileMap;

   typedef std::pair< std::vector<std::filesystem::path>, std::vector<std::filesystem::path> > TArrayFileArrayFolder;
   typedef std::map< std::filesystem::path, TArrayFileArrayFolder> TFolderMap;

   static std::shared_ptr<ProviderMemory> Factory(
      const TStaticFileMap& staticFiles = TStaticFileMap(),
      const TDynamicFileMap& dynamicFiles = TDynamicFileMap()
      );
private:
   struct TStaticFileData
   {
      TFileHash m_fileHash;
      TFileData m_fileData;
   };
   typedef std::shared_ptr< TStaticFileData > TPointerStaticFileData;
   struct TDynamicFileData
   {
      TFileData m_fileData;
   };
   typedef std::shared_ptr< TDynamicFileData > TPointerDynamicFileData;
   typedef ComponentFileMap< TPointerStaticFileData, TPointerDynamicFileData > TComponentFileMap;
   typedef std::shared_ptr< TComponentFileMap > TPointerComponentFileMap;
public:
   ProviderMemory(
      const TPointerComponentFileMap& pComponentFileMap
      );
   virtual ~ProviderMemory();

private:
   virtual void SetFilter(const int filter) override;
   virtual const int GetFilter()const override;
   virtual void SetFileSystemVisitorProvider(IFileSystemVisitorProvider* const pVisitor) override;
   virtual const bool QueryStaticFile(TFileHash& hashIfFound, const std::filesystem::path& path) override;
   virtual void AsyncLoadStaticFile(const TLoadCallback& loadCallback, const std::filesystem::path& path) override;

private:
   int m_filter;
   IFileSystemVisitorProvider* m_pVisitor;
   TPointerComponentFileMap m_pComponentFileMap;

};

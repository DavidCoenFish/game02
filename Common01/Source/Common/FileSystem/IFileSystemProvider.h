#pragma once

class FoundStaticFile;
class FoundStaticFolder;
class FoundDynamicFile;
class FoundDynamicFolder;
class IFileSystemVisitorProvider;

class IFileSystemProvider
{
public:
   typedef std::shared_ptr< std::vector< uint8_t > > TFileData;
   typedef std::function< void(const TFileData&) > TLoadCallback;
   typedef uint32_t TFileHash;

   virtual ~IFileSystemProvider();

   virtual void SetFilter(const int filter) = 0; //for FileSystem only?
   virtual const int GetFilter()const = 0;
   //onReady, onChangeStatic, onChangeDynamic? return a map of all the files/ locations, hash of static files... split dynamic & static
   virtual void SetFileSystemVisitorProvider(IFileSystemVisitorProvider* const pVisitor) = 0;

   virtual const bool QueryStaticFile(TFileHash& hashIfFound, const std::filesystem::path& path) = 0;
   //we don't care if the found hash changes between QueryStaticFile and AsyncLoadStaticFile calls, as we will have told the IFileSystemVisitorProvider that we changed
   virtual void AsyncLoadStaticFile(const TLoadCallback& loadCallback, const std::filesystem::path& path) = 0;

   virtual const bool QueryStaticFolder(const std::filesystem::path& path) = 0;
   virtual const bool GatherStaticFolderContents(
      std::vector< std::filesystem::path >& childFiles,
      std::vector< std::filesystem::path >& childFolders,
      const std::filesystem::path& path
      ) = 0;

   //virtual const bool SupportDynamic(void) const = 0;

   virtual void AddFoundStaticFile(FoundStaticFile* const pFoundStaticFile) = 0;
   virtual void RemoveFoundStaticFile(FoundStaticFile* const pFoundStaticFile) = 0;
   virtual void AddFoundStaticFolder(FoundStaticFolder* const pFoundStaticFolder) = 0;
   virtual void RemoveFoundStaticFolder(FoundStaticFolder* const pFoundStaticFolder) = 0;

   virtual void AddFoundDynamicFile(FoundDynamicFile* const pFoundDynamicFile) = 0;
   virtual void RemoveFoundDynamicFile(FoundDynamicFile* const pFoundDynamicFile) = 0;
   virtual void AddFoundDynamicFolder(FoundDynamicFolder* const pFoundDynamicFolder) = 0;
   virtual void RemoveFoundDynamicFolder(FoundDynamicFolder* const pFoundDynamicFolder) = 0;

};

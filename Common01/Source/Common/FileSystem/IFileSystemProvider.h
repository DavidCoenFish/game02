#pragma once

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

   //virtual const bool QueryStaticFolder(const std::filesystem::path& path) = 0;


};

#pragma once

/*
we can not create or delete static folders
changes in providers can however change what is in a static folder, add/ remove? static folders
ie, a mod turning on or off, and it providing a folder named "foo" and some files or folders inside that
*/
class IFileSystemProvider;
class IFileSystemVisitorFound;

class FoundStaticFolder
{
public:
   typedef std::shared_ptr< std::vector< uint8_t > > TFileData;
   typedef std::function< void(const TFileData&) > TLoadCallback;
   typedef uint32_t TFileHash;

   static std::shared_ptr< FoundStaticFolder > Factory(
      const std::filesystem::path& path, 
      const int filter,
      const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
      IFileSystemVisitorFound* pFileSystem
      );

   FoundStaticFolder(
      const std::filesystem::path& path, 
      const int filter,
      const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
      IFileSystemVisitorFound* pFileSystem
      );
   ~FoundStaticFolder();

   //when the function was called, the file existed
   const bool GetExist() const;

   const int GetFilter() const;
   void OnProviderChange(IFileSystemProvider* const pProvider);


private:
   //constant after ctor
   std::filesystem::path m_path;
   int m_filter;
   std::vector< std::weak_ptr< IFileSystemProvider > > m_arrayProvider;
   IFileSystemVisitorFound* m_pFileSystem;

   //can change any thread
   mutable std::mutex m_foundMutex;
   bool m_found;


};


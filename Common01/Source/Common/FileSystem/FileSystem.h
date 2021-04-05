#pragma once

class IReadOverlay;
class IWriteOverlay;

/*
there is a tendency to want access to the file system at various times during the lifetime of an application
   and rather than singelton or scoped object needing to be created ever earlier and earlier as other dependencies move, 
   moving towards a set of namespace functions with consumers that can be added and removed when appropriate

want to allow a zip file to be a read overlay, as well as dlc or mods possibly saved to locations on disk
want to allow multiple destination on save (cloud, temp file system)
either we implement async, or we need to be thread safe 
   risk of removing overlay during read?
   risk of having the same file handle open accross multiple threads, (OS may fail to open file in this case) 
have not committed to implementing a file cache, main consumer is shaders?
   on change of read overlay array, let them each in turn check the contents of the file cache incase they want to invalidate files?
   what if an overlay changes priority at runtime, invalidate cache of overlay?
   what if read and write work on same file, invalidate file cache for file on write?
   as well as a file data cache, what about what files exists
*/
namespace FileSystem
{
   const std::filesystem::path GetModualDir(HINSTANCE hInstance);
   const std::filesystem::path GetTempDir();

   //read is done by priority, file of a highest priority for path will be read
   void AddReadOverlay( const std::shared_ptr< IReadOverlay >& pOverlay );
   void RemoveReadOverlay( const std::shared_ptr< IReadOverlay >& pOverlay );
   void CleadReadOverlay();
   // or need to add/ remove overlay on change of priority
   //void OnOverlayChangePrioiry( const std::shared_ptr< IReadOverlay >& pOverlay );

   //write is done by filter
   void AddWriteOverlay( const std::shared_ptr< IWriteOverlay >& pOverlay );
   void RemoveWriteOverlay( const std::shared_ptr< IWriteOverlay >& pOverlay );
   void CleadWriteOverlay();

   //const bool GetFileExist(const std::filesystem::path& path) const;
   std::shared_ptr< std::vector<uint8_t> > GetFileData(const std::filesystem::path& path, const bool bCacheFile = false);
   std::shared_ptr< std::string > GetFileString(const std::filesystem::path& path, const bool bCacheFile = false);
   // or does something else handle the async and we need to be thread safe....
   //std::shared_ptr< AsyncJob< std::shared_ptr< std::vector<uint8_t> > > > GetFileDataAsync(const std::filesystem::path& path, const bool bCacheFile = false);

   //class FoundReadFile{ GetPath(); GetFileString(); GetFileData(); }
   //void GatherReadFiles(std::vector< std::shared_ptr< FoundReadFile >>& foundFiles, const std::filesystem::path& path);

   //write overlays, cloud? temp dir? local dir? is a 32 bit mask sufficient for all the write overlays
   const int SaveFileData(const int filter, const std::filesystem::path& path, const std::vector<uint8_t>& data, const bool bAppend = false);
   const int SaveFileString(const int filter, const std::filesystem::path& path, const std::string& data, const bool bAppend = false);
   const int DeleteSaveFile(const int filter, const std::filesystem::path& path);
   //std::shared_ptr< AsyncJob< int > > > SaveFileDataAsync(const int filter, const std::filesystem::path& path, const std::vector<uint8_t>& data);

   //void GatherWriteFiles(std::vector< std::shared_ptr< std::filesystem::path >>& foundFiles, const std::filesystem::path& path);

   //void ClearFileCacheFile(const std::filesystem::path& path);
   //void ClearFileCacheFile(const IReadOverlay& readOverlay);
   //void ClearFileCache();

   //std::map< std::filesystem::path, std::shared_ptr< FileCacheValue > > m_fileCache;

};

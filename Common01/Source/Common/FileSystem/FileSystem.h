#pragma once

class IReadOverlay;
class IWriteOverlay;

/*
want to allow a zip file to be a read overlay, as well as dlc or mods possibly saved to locations on disk
want to allow multiple destination on save (cloud, temp file system)
have not committed to implementing a file cache, main consumer is shaders?
   on change of read overlay array, let them each in turn check the contents of the file cache incase they want to invalidate files?
   what if an overlay changes priority at runtime, invalidate cache of overlay?
   what if read and write work on same file, invalidate file cache for file on write?
   as well as a file data cache, what about what files exists
either we implement async, or we need to be thread safe (risk of removing overlay during read?)
*/
class FileSystem
{
public:
   static const std::filesystem::path GetModualDir(HINSTANCE hInstance);
   static const std::filesystem::path GetTempPath();

   FileSystem();
   ~FileSystem();
   //read is done by priority, file of a higher priority will be read
   void AddReadOverlay( const std::shared_ptr< IReadOverlay >& pOverlay );
   void RemoveReadOverlay( const std::shared_ptr< IReadOverlay >& pOverlay );
   // or need to add/ remove overlay on change of priority
   //void OnOverlayChangePrioiry( const std::shared_ptr< IReadOverlay >& pOverlay );

   //write is done by filter
   void AddWriteOverlay( const std::shared_ptr< IWriteOverlay >& pOverlay );
   void RemoveWriteOverlay( const std::shared_ptr< IWriteOverlay >& pOverlay );

   //const bool GetFileExist(const std::filesystem::path& path) const;
   std::shared_ptr< std::vector<uint8_t> > GetFileData(const std::filesystem::path& path, const bool bCacheFile = false);
   std::shared_ptr< std::string > GetFileString(const std::filesystem::path& path, const bool bCacheFile = false);
   // or does something else handle the async and we need to be thread safe....
   //std::shared_ptr< AsyncJob< std::shared_ptr< std::vector<uint8_t> > > > GetFileDataAsync(const std::filesystem::path& path, const bool bCacheFile = false);

   //write overlays, cloud? temp dir? local dir? is a 32 bit mask sufficient for all the write overlays
   const int SaveFileData(const int filter, const std::filesystem::path& path, const std::vector<uint8_t>& data);
   const int SaveFileString(const int filter, const std::filesystem::path& path, const std::string& data);
   //std::shared_ptr< AsyncJob< int > > > SaveFileDataAsync(const int filter, const std::filesystem::path& path, const std::vector<uint8_t>& data);

   //void ClearFileCacheFile(const std::filesystem::path& path);
   //void ClearFileCacheFile(const IReadOverlay& readOverlay);
   //void ClearFileCache();

private:
   void SortReadOverlayArray();

private:
   //struct FileCacheValue
   //{
   //   std::shared_ptr< std::vector<uint8_t> > m_data;
   //   //we can not just save the prioirty, as a overlay's priority could change
   //   //std::weak_ptr< IReadOverlay > m_pOverlay;
   //   void* m_pOverlay; //want to id, but not to use... 
   //};

   std::vector< std::shared_ptr< IReadOverlay > > m_arrayOverlayRead;
   bool m_bArrayOverlayReadOrderDirty;
   std::vector< std::shared_ptr< IWriteOverlay > > m_arrayOverlayWrite;
   //std::map< std::filesystem::path, FileCacheValue> m_fileCache;

};

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
   risk of removing overlay during read? work around by limiting when overlay can be added/ removed?
   risk of having the same file handle open accross multiple threads, (OS may fail to get file handle in this case) 
have not committed to implementing a file cache, main consumer is shaders?
   on change of read overlay array, let them each in turn check the contents of the file cache incase they want to invalidate files?
   what if an overlay changes priority at runtime, invalidate cache of overlay?
   what if read and write work on same file, invalidate file cache for file on write?
   as well as a file data cache, what about what files exists
   (note, the operating system also keeps recently accessed files in memory....)
on changing overlay (add/ remove mods) call ReadClearFileCache()?

*/
namespace FileSystem
{
   const std::filesystem::path GetModualDir(HINSTANCE hInstance);
   const std::filesystem::path GetTempDir();

   const std::string DataToString(const std::shared_ptr< std::vector<uint8_t> >& pData);
   const std::string DataToString(const std::vector<uint8_t>& data);
   const std::vector<uint8_t> StringToData(const std::string& string);

   //read is done by priority, file of a highest priority for path will be read
   void AddReadOverlay( const std::shared_ptr< IReadOverlay >& pOverlay );
   void RemoveReadOverlay( const std::shared_ptr< IReadOverlay >& pOverlay );
   void ClearReadOverlay();

   //write is done by filter
   void AddWriteOverlay( const std::shared_ptr< IWriteOverlay >& pOverlay );
   void RemoveWriteOverlay( const std::shared_ptr< IWriteOverlay >& pOverlay );
   void ClearWriteOverlay();

   std::shared_ptr< std::vector<uint8_t> > ReadFileLoadData(const std::filesystem::path& path, const bool bCacheFile = false);
   void ReadClearFileCache();

   //write overlays, cloud? temp dir? local dir? is a 32 bit mask sufficient for all the write overlays
   const int WriteFileSaveData(const int filter, const std::filesystem::path& path, const std::vector<uint8_t>& data, const bool bAppend = false);
   const int WriteFileDelete(const int filter, const std::filesystem::path& path);

   //test exists/ search for file list of readable files by path, array of extentions. 
   //   how many functions? (test file exisits, search files in folder (with or without extentions?), get highest priority version of file accross overlay and extention array)
   //    std::shared_ptr< std::vector<uint8_t> > FoundFileData::ReadFileLoadData(), want a path to get back to the correct overlay to do the load
   //    std::filesystem::path FoundFileData::GetPath(), for display list of save games, GetFileName()
   //    FoundFileData::GetOverlayName? GetDateStamp?
   //std::shared_ptr< FoundFileData > ReadFileExists(file path)
   //std::shared_ptr< FoundFileData > ReadFileExistsExtentionFilter(file path no extention, extention priority filter array)
   //std::vector< std::shared_ptr< FoundFileData > > ReadFolderFileList(folder path)
   //std::vector< std::shared_ptr< FoundFileData > > ReadFolderFileListExtentionFilter(folder path, extention priority filter array)
};

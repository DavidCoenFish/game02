#pragma once

class IFileSystemOverlay;
class IFileSystemFoundFile;
class FileSystemImplimentation;

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

add mods collected into one read overlay, the intent is to add all the read overlays at application start
and not modify it over app lifespace (else need a sort hint)
this implies a "NotifyChangeReadOverlay" to let file system know mod changed

so, list of save files. want to read them, want to write them, may want to delete them

distinction between read only locations and writable location
overlay::
   CollectLocationsReadOnly
   CollectLocationsWritable

filesystem //don't load directly, get a IFileSystemFoundFile and load that
   FindFile(path)
   SaveFile(path, data)


IFileSystemFoundFile::
   async load


*/

class FileSystem
{
public:
   static const std::filesystem::path GetModualDir(HINSTANCE hInstance);
   static const std::filesystem::path GetTempDir();
   static const int GetFilterAll();
   static const int GetNewFilter();

   //this does infer that all overlays are avaliable at construction, but they could be placeholder, overlay can tell observer that contents have changed
   FileSystem(const std::vector<std::shared_ptr< IFileSystemOverlay > >& arrayOverlay);
   ~FileSystem();

   //void AddOverlay( const std::shared_ptr< IFileSystemOverlay >& pOverlay );
   //void RemoveOverlay( const std::shared_ptr< IFileSystemOverlay >& pOverlay );

   //tell the file system that the avaliable files has changed
   //void NotifyChangeOverlay( const std::shared_ptr< IFileSystemOverlay >& pOverlay );

   void LoadData( 
      const std::filesystem::path& path,
      const std::function< void(const std::shared_ptr< std::vector<uint8_t> >) >& callback = nullptr//,
      //const std::function< void() >& callbackError = nullptr
      );

   void SaveData(
      const std::shared_ptr< std::vector<uint8_t> >& data, 
      const std::filesystem::path& path, 
      const int filter = GetFilterAll(),
      const std::function< void() >& callback = nullptr//,
      //const std::function< void() >& callbackError = nullptr
      );

   //DeleteFile is a define
   void DeleteDataFile(
      const std::filesystem::path& path, 
      const int filter = GetFilterAll(),
      const std::function< void() >& callback = nullptr//,
      //const std::function< void() >& callbackError = nullptr
      );

   void FindFile(
      const std::filesystem::path& path, 
      const int filter = GetFilterAll(),
      const std::function< void(std::shared_ptr<IFileSystemFoundFile>) >& callback = nullptr
      );
   void FindFilePriorityExtention(
      const std::filesystem::path& path, 
      const std::vector< std::string >& extentionArrayHighestPriorityFirst,
      const int filter = GetFilterAll(),
      const std::function< void(std::shared_ptr<IFileSystemFoundFile>) >& callback = nullptr
      );
   void CollectFilesAtLocation(
      const std::filesystem::path& path, 
      const int filter = GetFilterAll(),
      const std::function< void(std::shared_ptr<IFileSystemFoundFile>) >& callback = nullptr
      );
 
   void RegisterWatchLocationChange(
      const std::filesystem::path& path, 
      const int filter = GetFilterAll(),
      const std::function< void() >& callback = nullptr
      );
   void ClearWatchLocationChange(
      const std::filesystem::path& path, 
      const int filter = GetFilterAll(),
      const std::function< void() >& callback = nullptr
      );

private:
   std::unique_ptr< FileSystemImplimentation > m_pImplimentation;

};








//class FileSystem
//{
//   const std::filesystem::path GetModualDir(HINSTANCE hInstance);
//   const std::filesystem::path GetTempDir();
//
//   const std::string DataToString(const std::shared_ptr< std::vector<uint8_t> >& pData);
//   const std::string DataToString(const std::vector<uint8_t>& data);
//   const std::vector<uint8_t> StringToData(const std::string& string);
//
//   const int GetOrRegisterFilter(const std::string& name);
//   const int GetFilterAllowAll();
//
//   //read is done by priority, file of a highest priority for path will be read
//   //write is done by filter
//   void AddOverlay( const std::shared_ptr< IFileSystemOverlay >& pOverlay, const int writeFilter);
//   void RemoveOverlay( const std::shared_ptr< IFileSystemOverlay >& pOverlay );
//   //tell the file system that the avaliable files has changed, it, mod being added/ removed
//   // this is to preserve the order of overlays/ avoid doing a "remove/add" to indicate change
//   void NotifyChangeOverlay( const std::shared_ptr< IFileSystemOverlay >& pOverlay );
//
//   std::shared_ptr< std::vector<uint8_t> > LoadData(const std::filesystem::path& path);
//   const int SaveData(const int filter, const std::filesystem::path& path, const std::vector<uint8_t>& data, const bool bAppend = false);
//   const int DeleteFile(const int filter, const std::filesystem::path& path);
// 
//   
//   
//   //deal both files and dirs?
//   std::shared_ptr< IFoundLocation > FindLocation(const std::filesystem::path& path);
//   //deal only files
//   std::shared_ptr< IFoundLocation > FindLocationFilePriorityExtention(
//      const std::filesystem::path& path,
//      const std::vector< std::string >& extentionArrayFirstHighest //
//      );
//
//   void GatherWritableFilesAtLocation(
//      std::vector< std::shared_ptr< IFoundLocation > >& out_foundFiles,
//      const std::filesystem::path& path,
//      const std::string& filterOrEmpty = std::string()
//      );
//   //ui may want to watch a data file or folder for mod changes
//   void RegisterLocationChange(
//      const std::shared_ptr< IFoundLocation >& pFoundLocation, 
//      const std::function<void(const std::shared_ptr< IFoundLocation >&)>& callback
//      );
//   std::shared_ptr< std::vector<uint8_t> > LoadFile(const std::filesystem::path& path);
//
//   //write overlays, cloud? temp dir? local dir? is a 32 bit mask sufficient for all the write overlays
//   const int SaveData(const int filter, const std::filesystem::path& path, const std::vector<uint8_t>& data, const bool bAppend = false);
//   const int DeleteWritableFile(const int filter, const std::filesystem::path& path);
//
//   //test exists/ search for file list of readable files by path, array of extentions. 
//   //   how many functions? (test file exisits, search files in folder (with or without extentions?), get highest priority version of file accross overlay and extention array)
//   //    std::shared_ptr< std::vector<uint8_t> > FoundFileData::ReadFileLoadData(), want a path to get back to the correct overlay to do the load
//   //    std::filesystem::path FoundFileData::GetPath(), for display list of save games, GetFileName()
//   //    FoundFileData::GetOverlayName? GetDateStamp?
//   //std::shared_ptr< FoundFileData > ReadFileExists(file path)
//   //std::shared_ptr< FoundFileData > ReadFileExistsExtentionFilter(file path no extention, extention priority filter array)
//   //std::vector< std::shared_ptr< FoundFileData > > ReadFolderFileList(folder path)
//   //std::vector< std::shared_ptr< FoundFileData > > ReadFolderFileListExtentionFilter(folder path, extention priority filter array)
//};

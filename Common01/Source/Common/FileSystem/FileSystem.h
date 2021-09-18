#pragma once

class IFileSystemProvider;
class FileSystemInternal;
class FoundStaticFile;
class FoundStaticFolder;
class FoundDynamicFile;
class FoundDynamicFolder;

/*
dynamic and static not exist in the same mapping of the filesystem, and both use relative paths?
"best" is in reference to the higher the index of the provider, the higher the priority which makes it "Better". filter mask = 1 << index
"all" is to allow action on every provider that passes the filter mask

may not need to change avaliablity/ order of overlays at runtime, thought that may result in mods having to do more work, 
   ie. one mod provider that internally manages mod order and being on/off
   this could look like having the file system created early, with placeholders provider, which lets observers know when things change/ are ready?

do the providers use the file system AddAsyncTask to do startup work, or it that the application's job


IFileSystemProvider
   SetFilter() //for FileSystem only?
   GetFilter()
   SetCallbacks(ready, change) //onReady/onChange return a map of all the files/ locations, hash of static files...
   AsyncStaticFileLoad()
   AsyncDynamicFileLoad()
   AsyncDynamicFileSave()
   AsyncDynamicFileDelete()
   AsyncDynamicFolderMake() //so, does this update map of all files, it probably should, are we going to end up with static and dynamic maps of files?
   AsyncDynamicFolderRemove()


FoundStaticFile //must exisit to be returned
   AsyncLoadBest,
   // trigger callback if the best files has a diffierent hash, callback triggers as soon as run as you don't start with a file, thus it has changed?
   AddCallbackChangeBest(path, filter, callback), //first thing it does is return the current file data? redundant with AsyncLoadBest but keeps data in step with changes
   RemoveCallback(path, filter, callback)
   //AsyncLoadAll, is needed?

FoundStaticFolder //must exisit to be returned
   GatherAllChildStaticFiles
   AddCallbackChangeStaticFiles
   GatherAllChildStaticFolders
   AddCallbackChangeStaticFolders

FoundDynamicFile 
   GetExisits
   AsyncDelete,
   AsyncLoadBest,
   AddCallbackChangeBest(), //first thing it does is return the current file data? redundant but keeps data in step with changes
   //AddCallbackChangeAll(), 
   //AsyncLoadAll, is this even wanted
   AsyncSaveAll,
   //AsyncSaveBest, is this even wanted
   AsyncDeleteAll,
   //AsyncDeleteAll, is this even wanted
   
FoundDynamicFolder
   GetExisits
   AsyncMake,
   AsyncRemove,
   GatherAllChildDynamicFiles
   AddCallbackChangeDynamicFiles
   GatherAllChildDynamicFolders
   AddCallbackChangeDynamicFolders

   */

class FileSystem
{
public:
   typedef std::shared_ptr< std::vector< uint8_t > > TFileData;
   typedef std::function< void(void) > TVoidCallback;
   typedef uint32_t TFileHash;

   // we construct the file system with an array of providers, however they need to tell the file system when they are ready
   static std::shared_ptr< FileSystem > Factory(const std::vector<std::shared_ptr< IFileSystemProvider > >& arrayOverlay);

   static const std::filesystem::path GetModualDir(HINSTANCE hInstance);
   static const std::filesystem::path GetTempDir();
   static const int GetFilterAll();

   FileSystem(const std::vector<std::shared_ptr< IFileSystemProvider > >& arrayOverlay);
   ~FileSystem();

   //static meaning they can not be edited, a provider may change what the static file provided is however
   // static "/test.txt" <foo> from provider a, when provider b is ready, it may provide static "/test.txt" <foo2>

   //we can find files/ folders that don't exist (yet) instead of FoundFile -> HandelFile?

   std::shared_ptr< FoundStaticFile > FindStaticFile(const std::filesystem::path& path, const int filter = GetFilterAll());
   std::shared_ptr< FoundStaticFile > FindStaticFilePriorityExtention(const std::filesystem::path& path, const std::vector<std::string>& priorityExtention, const int filter = GetFilterAll());
   std::shared_ptr< FoundStaticFolder > FindStaticFolder(const std::filesystem::path& path, const int filter = GetFilterAll());

   std::shared_ptr< FoundDynamicFile > FindDynamicFile(const std::filesystem::path& path, const int filter = GetFilterAll());
   std::shared_ptr< FoundDynamicFolder > FindDynamicFolder(const std::filesystem::path& path, const int filter = GetFilterAll());

   // only callsback on "all" the providers of filter being ready?
   // filter bit [2, 4], then providers [2,4] must be ready before callback is called
   void AddCallbackAllProvidersReady(const TVoidCallback& callback, const int filter = GetFilterAll());

   /*
   //helper functions to save people from having to do a lot of work for basic functions
   // load the file best matching provider
   // WARNING, is this safe to use if file will just get replace by mod-ed version latter? we might not care?
   void AsyncStaticFileLoadBest(const std::filesystem::path& path, const int filter, const std::function<const TFileData(void)>& callback);
   // ie, load a json file of what mods are enabled/ disabled and where installed... might only have local file system filter...
   void AsyncDynamicFileLoadBest(const std::filesystem::path& path, const int filter, const std::function<const TFileData(void)>& callback);
   // save to all provider that pass filter
   void AsyncDynamicFileSaveAll(const std::filesystem::path& path, const int filter, const std::function<const TFileData(void)>& callback);
   */

private:
   std::unique_ptr< FileSystemInternal > m_pInternal;

};

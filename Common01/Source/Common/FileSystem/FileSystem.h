#pragma once

class IFileSystemOverlay;
class FileSystemInternal;
class FoundFile;
class FoundLocation;

/*
there is a tendency to want access to the file system at various times during the lifetime of an application
   and rather than singelton or scoped object needing to be created ever earlier and earlier as other dependencies move, 
   moving towards a set of namespace functions with consumers that can be added and removed when appropriate

may not need to change avaliablity/ order of overlays at runtime, thought that may result in mods having to do more work, 
   ie. one mod overlay that internally manages mod order and being on/off
   this could look like having the file system created early, with placeholders overlays, which lets observers know when things change/ are ready?

can you find a file before it exists, to save it? after deleting it?

do not want two different behaviours for the filter.
   on load, we load the file that matches the last overlay with the file
   on save, was going to save the file on every overlay that passed filter.
   but we don't load the file from every overlay that passes.
   so does save/ delete work like load? 

*/

class FileSystem
{
public:
   static const std::filesystem::path GetModualDir(HINSTANCE hInstance);
   static const std::filesystem::path GetTempDir();
   static const int GetFilterAll();
   static const int GetNewFilter();

public:
   static std::shared_ptr< FileSystem > Factory(const std::vector<std::shared_ptr< IFileSystemOverlay > >& arrayOverlay);
   FileSystem(const std::vector<std::shared_ptr< IFileSystemOverlay > >& arrayOverlay);
   ~FileSystem();

   //shortcut for FindFile -> AsyncLoadData, highest priority file that passes overlay filter
   void AsyncLoadData( 
      const std::filesystem::path& path,
      const std::function<void(const bool success, const int filter, const std::shared_ptr<std::vector<uint8_t>>& data)>& callback,
      const int filter = GetFilterAll()
      );

   //shortcut for FindFile -> AsyncSaveData, save the data on filtered overlays
   void AsyncSaveData(
      const std::shared_ptr< std::vector<uint8_t> >& data, 
      const std::filesystem::path& path, 
      const std::function< void(const bool success, const int filter) >& callback = nullptr,
      const int filter = GetFilterAll()
      );

   //delete the file on the filtered overlays
   void AsyncDeleteFile(
      const std::filesystem::path& path, 
      const std::function< void(const bool success, const int filter) >& callback = nullptr,
      const int filter = GetFilterAll()
      );

   //find the highest priority file that passes overlay filter, can then load, delete? the file
   // to accomidate save, file may not exists?
   const std::shared_ptr<FoundFile> FindFile(
      const std::filesystem::path& path, 
      const int filter = GetFilterAll()
      );
   const std::shared_ptr<FoundFile> FindFilePriorityExtention(
      const std::filesystem::path& path, 
      const std::vector< std::string >& extentionArrayHighestPriorityFirst,
      const int filter = GetFilterAll()
      );

   const std::shared_ptr<FoundLocation> FindLocation(
      const std::filesystem::path& path, 
      const int filter = GetFilterAll()
      );

private:
   std::shared_ptr< FileSystemInternal > m_pInternal;

};

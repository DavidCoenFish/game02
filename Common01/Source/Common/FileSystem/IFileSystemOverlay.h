#pragma once

/*
is there an easier way or do you have to filter out the cloud overlay from searches?
(for the load files from disk or mod overlay)

2 cases, 
   mod turned on and off (avaliable files change)
   save a game to a dir, want list of files in dir. dont cache the files in "dynamic" dirs?
      dont notify observer that contents have changes on saving game?

*/
class IFileSystemFoundFile;
class IFileSystemOverlay
{
public:
   virtual ~IFileSystemOverlay();

   virtual void SetCallbackContentChanged(const std::function<void()>& callback) = 0;

   virtual const int GetFilter() = 0;

   //does this overlay have files under the following location
   virtual const bool LocationFound(const std::filesystem::path& path) = 0;
   //does this overlay have 
   virtual const bool FileFound(const std::filesystem::path& path) = 0;

   // have the FileSystem make a path of each of the extentionArrayHighestPriorityFirst can call FileFound
   //virtual const bool FileFoundPriorityExtention(
   //   const std::filesystem::path& path,
   //   const std::vector< std::string >& extentionArrayHighestPriorityFirst,
   //   std::filesystem::path& foundPath
   //   ) = 0;

   virtual void LoadData(
      const std::filesystem::path& path,
      const std::function< void(const std::shared_ptr< std::vector<uint8_t> >) >& callback
      ) = 0;

   virtual void SaveData(
      const std::shared_ptr< std::vector<uint8_t> >& data, 
      const std::filesystem::path& path, 
      const std::function< void() >& callback
      ) = 0;

   virtual void DeleteDataFile(
      const std::filesystem::path& path, 
      const std::function< void() >& callback
      ) = 0;

   virtual void CollectFilesAtLocation(
      const std::shared_ptr<IFileSystemOverlay>& refSelf,
      const std::filesystem::path& path, 
      std::map< std::filesystem::path, std::shared_ptr<IFileSystemFoundFile>>& mapFoundFiles
      ) = 0;



};

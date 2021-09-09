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

   virtual const int GetFilter() const = 0;

   //let filesystem know if something has changed?
   virtual void SetCallbackChangedFile(const std::function<void(const std::filesystem::path&, IFileSystemOverlay* const)>& callback) = 0;
   virtual void SetCallbackChangedLocation(const std::function<void(const std::filesystem::path&, IFileSystemOverlay* const)>& callback) = 0;

   //work with FoundFile, doesn't this need to be async, when does the overlay get the time? or does a disk overlay have a callback for init make file map finished?
   virtual const bool ProvidesFile(const std::filesystem::path& path) = 0;

   //work with FoundLocation
   virtual const bool ProvidesLocation(const std::filesystem::path& path) = 0;

   //help populate FoundLocation::CollectFiles, async?
   virtual void AsyncVisitFilesUnderPath(
      const std::filesystem::path& path,
      const std::function<void(const std::filesystem::path& path)>& callback
      ) = 0;

   //filestamp?
   virtual void AsyncLoadData( 
      const std::filesystem::path& path,
      const std::function< void(const bool success, const int filter, const std::shared_ptr<std::vector<uint8_t>>& data) >& callback
      ) = 0;

   //async?
   //virtual void GetFileChangeID(const std::filesystem::path& path) const= 0;
   virtual void AsyncGetFileChangeID(
      const std::filesystem::path& path,
      const std::function< void(const int changeID) >& callback
      ) const= 0;

   //ChangeID?
   virtual void AsyncSaveData(
      const std::filesystem::path& path,
      const std::shared_ptr< std::vector<uint8_t> >& data, 
      const std::function< void(const bool success, const int filter) >& callback = nullptr
      ) = 0;

   //ChangeID? timestamp of when file deleted, or 0 if never existed?
   virtual void AsyncDeleteFile(
      const std::filesystem::path& path,
      const std::function< void(const bool success, const int filter) >& callback = nullptr
      ) = 0;

};

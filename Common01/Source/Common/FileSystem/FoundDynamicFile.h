#pragma once

/*
do files have a change id? a hash? what if contents of folder change but not the file. 
what if different file from a differnt overlay is now the priority file (file stamp{ mOverlay*, changeId }?)
overlay provides the file

what is the flow of information if the mod overlay turns on a mod that provides a higher priority verion of our found file
 file "root/text.txt" found in base overlay "filesystem"
 overlay mod updated to provide a higher priority version of "root/text.txt"

does the mod nominate that location root/ has changed? 
does the file system have a list of std::filesystem::path that have been passed out to FoundFiles?
or just a list of active foundfiles?
the file system could maintain a list all all known files, locations, and overlay/ changeid stack?
would work on read files, but what about write? they may or may not exists.
or is there one copy of the data and that is kept in all the overlays, and the files system visits as required

found file can be told an overlay has changed?, the found file might only care if that overlay is it's highest priority provider..?

*/

class IFileSystemOverlay;
class FileSystemInternal;

class FoundFile
{
public:
   struct FileStamp
   {
      FileStamp(void* pOverlay, int changeID);

      void* mOverlay;
      int mChangeID; //last modified? change counter?
   };

   ~FoundFile();

   const bool Exisits() const;

   //FileStamp? ChangeID?
   void AsyncLoadData( 
      const std::function< void(const bool success, const int filter, const std::shared_ptr< std::vector<uint8_t> >& pData) >& callback
      );

   //
   void AsyncSaveData(
      const std::shared_ptr< std::vector<uint8_t> >& data, 
      const std::function< void(const bool success, const int filter) >& callback = nullptr
      );

   void AsyncDeleteFile(
      const std::function< void(const bool success, const int filter) >& callback = nullptr
      );

   const FileStamp GetFileStamp() const;

   //check if the file has changed
   const bool NeedUpdate(FileStamp& token) const;

   //be told if the file changes
   void AddWatch(
      const std::function<void(const FileStamp&)>& callback
      );

private:
   std::weak_ptr< FileSystemInternal > m_pFileSystemInternal;
   std::vector< std::weak_ptr< IFileSystemOverlay > > m_overlayArray;
   std::weak_ptr< IFileSystemOverlay > m_pLastOverlay;
   int m_changeID;
   std::filesystem::path m_path;

   //do we have a std::vector< FileStamp > m_stackFileStamp; 
};

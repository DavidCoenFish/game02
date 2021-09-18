#pragma once

class ComponentFileMap
{
public:
   typedef uint32_t TFileHash;

   static std::unique_ptr< ComponentFileMap > Factory();
   ComponentFileMap(const ComponentFileMap& src);
   ComponentFileMap();
   ~ComponentFileMap();

   void Clear();
   void AddStaticFile(const std::filesystem::path& path, const TFileHash hash);
   void AddStaticFolder(const std::filesystem::path& path);

   //do dynamic files have a hash? file can be modified run time, do we want to regen hash? not really
   void AddDynamicFile(const std::filesystem::path& path);
   void AddDynamicFolder(const std::filesystem::path& path);

   //get static file exisits/ hash
   //gather static folder child files, folders

   //get dynamic file exisits
   //gather dynamic folder child files, folders

private:
   std::map< std::filesystem::path, TFileHash > m_mapStaticFiles;
   std::map< std::filesystem::path, std::shared_ptr< std::set< std::filesystem::path > > > m_mapStaticFolderFiles;
   std::map< std::filesystem::path, std::shared_ptr< std::set< std::filesystem::path > > > m_mapStaticFolderFolder;

   std::set< std::filesystem::path> m_mapDynamicFiles;
   std::map< std::filesystem::path, std::shared_ptr< std::set< std::filesystem::path > > > m_mapDynamicFolderFiles;
   std::map< std::filesystem::path, std::shared_ptr< std::set< std::filesystem::path > > > m_mapDynamicFolderFolder;

};

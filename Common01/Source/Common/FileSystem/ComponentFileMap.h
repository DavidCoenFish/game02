#pragma once

class ComponentFileMap
{
public:
   static std::unique_ptr< ComponentFileMap > Factory();
   ComponentFileMap(const ComponentFileMap& src);
   ComponentFileMap();
   ~ComponentFileMap();

   void Clear();
   void AddFile(const std::filesystem::path& path);
   const bool HasFile(const std::filesystem::path& path);
   const bool HasDir(const std::filesystem::path& dir);
   void VisitFilesInDir(
      const std::filesystem::path& dir,
      const std::function< void(const std::filesystem::path&) >& visitor
      );

   const ComponentFileMap& operator=(const ComponentFileMap& src);
   //overlay one file map on another 
   void Overlay(const ComponentFileMap& src);

private:
   std::set< std::filesystem::path > m_mapFiles;
   std::map< std::filesystem::path, std::shared_ptr< std::set< std::filesystem::path > > > m_mapDirFiles;

};

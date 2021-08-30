#pragma once

class ComponentFileMap
{
public:
   static std::unique_ptr< ComponentFileMap > Factory();
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

private:
   std::set< std::filesystem::path > m_mapFiles;
   std::map< std::filesystem::path, std::shared_ptr< std::set< std::filesystem::path > > > m_mapDirFiles;

};

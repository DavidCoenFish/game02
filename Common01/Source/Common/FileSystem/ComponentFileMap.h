#pragma once
/*
template the data for static file and dynamic file to allow this to be usable by providers?

DiskProvider 
static file data { TFileHash }

MemoryProvider
static/dynamic file data { TFileHash, shared_ptr< vector< uint8_t > > }

*/
struct TComponentFileMapEmpty
{
};

template <typename TStaticFileData = TComponentFileMapEmpty, typename TDynamicFileData = TComponentFileMapEmpty >
class ComponentFileMap
{
public:
   typedef uint32_t TFileHash;

   static std::unique_ptr< ComponentFileMap > Factory()
   {
      return std::make_shared< ComponentFileMap >();
   }
   //ComponentFileMap(const ComponentFileMap& src);
   ComponentFileMap()
   {
      //nop
   }
   ~ComponentFileMap()
   {
      //nop
   }

   void Clear()
   {
      m_mapStaticFiles.clear();
      m_mapStaticFolderFiles.clear();
      m_mapStaticFolderFolder.clear();
      m_mapDynamicFiles.clear();
      m_mapDynamicFolderFiles.clear();
      m_mapDynamicFolderFolder.clear();
   }

   void AddStaticFile(const std::filesystem::path& path, const TStaticFileData& data = nullptr)
   {
      m_mapStaticFiles[path] = data;
      AddFileToFolders(path, m_mapStaticFolderFiles, m_mapStaticFolderFolder);
      return;
   }

   const bool GetStaticFile(const std::filesystem::path& path, TStaticFileData& data)
   {
      auto found = m_mapStaticFiles.find(path);
      if (found == m_mapStaticFiles.end())
      {
         return false;
      }
      data = found->second;
      return true;
   }

   void AddStaticFolder(const std::filesystem::path& path)
   {
      AddFolderToFolders(path, m_mapStaticFolderFolder);
      return;
   }

   void AddDynamicFile(const std::filesystem::path& path, const TDynamicFileData& data = nullptr)
   {
      m_mapDynamicFiles[path] = data;
      AddFileToFolders(path, m_mapDynamicFolderFiles, m_mapDynamicFolderFolder);
      return;
   }

   const bool GetDynamicFile(const std::filesystem::path& path, TDynamicFileData& data)
   {
      auto found = m_mapDynamicFiles.find(path);
      if (found == m_mapDynamicFiles.end())
      {
         return false;
      }
      data = found.second;
      return true;
   }

   void AddDynamicFolder(const std::filesystem::path& path)
   {
      AddFolderToFolders(path, m_mapDynamicFolderFolder);
      return;
   }

   //get static file exisits/ hash
   //gather static folder child files, folders
   void GatherStaticFolder(const std::filesystem::path& path, std::vector<std::filesystem::path>& arrayFiles, std::vector<std::filesystem::path>& arrayFolders)
   {
      for (const auto& item : m_mapStaticFolderFiles)
      {
         arrayFiles.push_back(item);
      }
      for (const auto& item : m_mapStaticFolderFolder)
      {
         arrayFolders.push_back(item);
      }
      return;
   }

   //get dynamic file exisits
   //gather dynamic folder child files, folders
   void GatherDynamicFolder(const std::filesystem::path& path, std::vector<std::filesystem::path>& arrayFiles, std::vector<std::filesystem::path>& arrayFolders)
   {
      for (const auto& item : m_mapDynamicFolderFiles)
      {
         arrayFiles.push_back(item);
      }
      for (const auto& item : m_mapDynamicFolderFolder)
      {
         arrayFolders.push_back(item);
      }
      return;
   }

private:
   static void AddFileToFolders(
      const std::filesystem::path& path, 
      std::map< std::filesystem::path, std::shared_ptr< std::set< std::filesystem::path > > >& mapFolderFiles,
      std::map< std::filesystem::path, std::shared_ptr< std::set< std::filesystem::path > > >& mapFolderFolder
      )
   {
      //std::filesystem::path fileName = path.filename();
      std::filesystem::path trace = path.parent_path();
      if (true == trace.empty())
      {
         return;
      }
      auto found = mapFolderFiles.find(trace);
      if (found != mapFolderFiles.end())
      {
         found->second->insert( path );
      }
      AddFolderToFolders(trace, mapFolderFolder);
   }

   static void AddFolderToFolders(
      const std::filesystem::path& path, 
      std::map< std::filesystem::path, std::shared_ptr< std::set< std::filesystem::path > > >& mapFolderFolder
      )
   {
      std::filesystem::path tracePrev = path;
      std::filesystem::path trace = path.parent_path();
      while (tracePrev != trace)
      {
         auto found = mapFolderFolder.find(trace);
         std::shared_ptr< std::set< std::filesystem::path > > pSet;
         if (found != mapFolderFolder.end())
         {
            pSet = found->second;
         }
         else
         {
            pSet = std::make_shared< std::set< std::filesystem::path > >();
            mapFolderFolder[ trace ] = pSet;
         }
         pSet->insert(tracePrev);
   
         tracePrev = trace;
         trace = trace.parent_path();
      }
      return;
   }

private:
   std::map< std::filesystem::path, TStaticFileData > m_mapStaticFiles;
   std::map< std::filesystem::path, std::shared_ptr< std::set< std::filesystem::path > > > m_mapStaticFolderFiles;
   std::map< std::filesystem::path, std::shared_ptr< std::set< std::filesystem::path > > > m_mapStaticFolderFolder;

   std::map< std::filesystem::path, TDynamicFileData > m_mapDynamicFiles;
   std::map< std::filesystem::path, std::shared_ptr< std::set< std::filesystem::path > > > m_mapDynamicFolderFiles;
   std::map< std::filesystem::path, std::shared_ptr< std::set< std::filesystem::path > > > m_mapDynamicFolderFolder;

};

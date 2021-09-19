#pragma once
/*
change to just one set of data, could make a non thread safe version of this
for performance if used in situation without runtime editing on different threads...


template the data for static file and dynamic file to allow this to be usable by providers?

this class is not thread safe, it is not intended to be, only set on creation.
but the point of dynamic is that they may change, and rebuild is expencive, so we have a 
ComponentFileMap, and a ComponentDynamicFileMap

DiskProvider 
static file data { TFileHash }

MemoryProvider
static/dynamic file data { TFileHash, shared_ptr< vector< uint8_t > > }

*/
struct TComponentFileMapEmpty
{
};

template <typename TFileData = TComponentFileMapEmpty>
class ComponentFileMap
{
public:
   typedef std::map< std::filesystem::path, TFileData > TMapPathFileData;
   static std::shared_ptr< ComponentFileMap > Factory(
      const TMapPathFileData& mapFiles = TMapPathFileData()
      )
   {
      return std::make_shared< ComponentFileMap >(mapFiles);
   }
   //ComponentFileMap(const ComponentFileMap& src);
   ComponentFileMap(const TMapPathFileData& mapFiles)
      : m_mapFiles(mapFiles)
   {
      for (const auto& iter : mapFiles)
      {
         AddFileToFolders(iter.first);
      }
   }
   ~ComponentFileMap()
   {
      //nop
   }

   void Clear()
   {
      {
         std::lock_guard lock(m_mapFilesMutex);
         m_mapFiles.clear();
      }
      {
         std::lock_guard lock(m_mapFolderFilesMutex);
         m_mapFolderFiles.clear();
      }
      {
         std::lock_guard lock(m_mapFolderFolderMutex);
         m_mapFolderFolder.clear();
      }
   }

   void AddFile(const std::filesystem::path& path, const TFileData& data)
   {
      {
         std::lock_guard lock(m_mapFilesMutex);
         m_mapFiles[path] = data;
      }
      AddFileToFolders(path);
      return;
   }

   const bool GetFile(const std::filesystem::path& path, TFileData& data)
   {
      std::lock_guard lock(m_mapFilesMutex);
      auto found = m_mapFiles.find(path);
      if (found == m_mapFiles.end())
      {
         return false;
      }
      data = found->second;
      return true;
   }

   void AddFolder(const std::filesystem::path& path)
   {
      AddFolderToFolders(path);
      return;
   }

   const bool HasFolder(const std::filesystem::path& path)
   {
      {
         std::lock_guard lock(m_mapFolderFilesMutex);
         const auto iter = m_mapFolderFiles.find(path);
         if (iter != m_mapFolderFiles.end())
         {
            return true;
         }
      }
      {
         std::lock_guard lock(m_mapFolderFolderMutex);
         const auto iter = m_mapFolderFolder.find(path);
         if (iter != m_mapFolderFolder.end())
         {
            return true;
         }
      }
      return false;
   }

   const bool GatherFolder(
      const std::filesystem::path& path, 
      std::vector<std::filesystem::path>& arrayFiles, 
      std::vector<std::filesystem::path>& arrayFolders
      )
   {
      bool found = false;
      //std::map< std::filesystem::path, std::shared_ptr< std::set< std::filesystem::path > > > m_mapFolderFiles;
      {
         std::lock_guard lock(m_mapFolderFilesMutex);
         const auto iter = m_mapFolderFiles.find(path);
         if (iter != m_mapFolderFiles.end())
         {
            found = true;
            for (const auto& item : *(iter->second))
            {
               arrayFiles.push_back(item);
            }
         }
      }
      {
         std::lock_guard lock(m_mapFolderFolderMutex);
         const auto iter = m_mapFolderFolder.find(path);
         if (iter != m_mapFolderFolder.end())
         {
            found = true;
            for (const auto& item : *(iter->second))
            {
               arrayFolders.push_back(item);
            }
         }
      }
      return found;
   }

   void RemoveFile(const std::filesystem::path& path)
   {
      {
         std::lock_guard lock(m_mapFilesMutex);
         auto found = m_mapFiles.find(path);
         if (found == m_mapFiles.end())
         {
            return;
         }
         m_mapFiles.erase(found);
      }
      {
         std::filesystem::path trace = path.parent_path();
         if (true == trace.empty())
         {
            return;
         }
         std::lock_guard lock(m_mapFolderFilesMutex);
         auto found = m_mapFolderFiles.find(trace);
         if (found == m_mapFolderFiles.end())
         {
            return;
         }
         m_mapFolderFiles.erase(found);
      }
   }

   void RemoveFolder(const std::filesystem::path& path)
   {
      std::shared_ptr< std::set< std::filesystem::path > > filesToRemove;
      {
         std::lock_guard lock(m_mapFolderFilesMutex);
         auto found = m_mapFolderFiles.find(path);
         if (found != m_mapFolderFiles.end())
         {
            filesToRemove = found->second;
            m_mapFolderFiles.erase(found);
         }
      }
      std::shared_ptr< std::set< std::filesystem::path > > foldersToRemove;
      {
         std::lock_guard lock(m_mapFolderFolderMutex);
         auto found = m_mapFolderFolder.find(path);
         if (found != m_mapFolderFolder.end())
         {
            foldersToRemove= found->second;
            m_mapFolderFolder.erase(found);
         }
      }
      {
         std::lock_guard lock(m_mapFilesMutex);
         for (const auto& iter : filesToRemove)
         {
            auto found = m_mapFiles.find(iter);
            if (found != m_mapFiles.end())
            {
               m_mapFiles.erase(found);
            }
         }
      }
      for (const auto& iter : foldersToRemove)
      {
         RemoveFolder(iter);
      }
      return;
   }

private:
   void AddFileToFolders(
      const std::filesystem::path& path
      )
   {
      //std::filesystem::path fileName = path.filename();
      std::filesystem::path trace = path.parent_path();
      if (true == trace.empty())
      {
         return;
      }
      {
         std::lock_guard lock(m_mapFolderFilesMutex);
         auto found = m_mapFolderFiles.find(trace);
         std::shared_ptr<std::set< std::filesystem::path >> pSet;
         if (found != m_mapFolderFiles.end())
         {
            pSet = found->second;
         }
         else
         {
            pSet = std::make_shared<std::set< std::filesystem::path >>();
            m_mapFolderFiles[trace] = pSet;
         }
         pSet->insert(path);
      }
      AddFolderToFolders(trace);
   }

   void AddFolderToFolders(
      const std::filesystem::path& path
      )
   {
      std::filesystem::path tracePrev = path;
      std::filesystem::path trace = path.parent_path();
      while (tracePrev != trace);
      {
         {
            std::lock_guard lock(m_mapFolderFolderMutex);
            auto found = m_mapFolderFolder.find(trace);
            std::shared_ptr< std::set< std::filesystem::path > > pSet;
            if (found != m_mapFolderFolder.end())
            {
               pSet = found->second;
            }
            else
            {
               pSet = std::make_shared< std::set< std::filesystem::path > >();
               m_mapFolderFolder[ trace ] = pSet;
            }
            pSet->insert(tracePrev);
         }
   
         tracePrev = trace;
         trace = trace.parent_path();
      }
      return;
   }

private:
   std::mutex m_mapFilesMutex;
   TMapPathFileData m_mapFiles;
   std::mutex m_mapFolderFilesMutex;
   std::map< std::filesystem::path, std::shared_ptr< std::set< std::filesystem::path > > > m_mapFolderFiles;
   std::mutex m_mapFolderFolderMutex;
   std::map< std::filesystem::path, std::shared_ptr< std::set< std::filesystem::path > > > m_mapFolderFolder;


};

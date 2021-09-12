#include "CommonPCH.h"

#include "Common/FileSystem/ComponentFileMap.h"

std::unique_ptr< ComponentFileMap > ComponentFileMap::Factory()
{
   return std::make_unique< ComponentFileMap >();
}

ComponentFileMap::ComponentFileMap()
{
   return;
}

ComponentFileMap::~ComponentFileMap()
{
   return;
}

void ComponentFileMap::Clear()
{
   return;
}

//void ComponentFileMap::AddFile(const std::filesystem::path& path)
//{
//   m_mapFiles.insert(path);
//
//   std::filesystem::path tracePrev = path;
//   std::filesystem::path trace = path.parent_path();
//   bool first = true;
//   while (tracePrev != trace)
//   {
//      auto found = m_mapDirFiles.find(trace);
//      std::shared_ptr< std::set< std::filesystem::path > > pSet;
//      if (found != m_mapDirFiles.end())
//      {
//         pSet = found->second;
//      }
//      else
//      {
//         pSet = std::make_shared< std::set< std::filesystem::path > >();
//         m_mapDirFiles[ trace ] = pSet;
//      }
//      if (true == first)
//      {
//         first = false;
//         pSet->insert(path);
//      }
//
//      tracePrev = trace;
//      trace = trace.parent_path();
//   }
//   return;
//}
//
//const bool ComponentFileMap::HasFile(const std::filesystem::path& path)
//{
//   return (m_mapFiles.end() != m_mapFiles.find(path));
//}
//
//const bool ComponentFileMap::HasDir(const std::filesystem::path& dir)
//{
//   return (m_mapDirFiles.end() != m_mapDirFiles.find(dir));
//}
//
//void ComponentFileMap::VisitFilesInDir(
//   const std::filesystem::path& dir,
//   const std::function< void(const std::filesystem::path&) >& visitor
//   )
//{
//   auto found = m_mapDirFiles.find(dir);
//   if (found != m_mapDirFiles.end())
//   {
//      for (const auto& iter : *(found->second))
//      {
//         visitor(iter);
//      }
//   }
//   return;
//}

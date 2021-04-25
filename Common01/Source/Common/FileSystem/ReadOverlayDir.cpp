#include "CommonPCH.h"

#include "Common/FileSystem/ReadOverlayDir.h"

ReadOverlayDir::ReadOverlayDir(const int priority, const std::filesystem::path& basePath)
   : m_priority(priority)
   , m_basePath(basePath)
{
   //nop
}

ReadOverlayDir::~ReadOverlayDir()
{
   //nop
}

std::shared_ptr< std::vector<uint8_t> > ReadOverlayDir::ReadFileLoadData(const std::filesystem::path& path)
{
   const auto localPath = m_basePath / path;
   std::ifstream stream(localPath.c_str(), std::ios::in | std::ios::binary);
   if (false == stream.is_open())
   {
      return nullptr;
   }
   auto pResult = std::make_shared<std::vector<uint8_t>>((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
   return pResult;
}

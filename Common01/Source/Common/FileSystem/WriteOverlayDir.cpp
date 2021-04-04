#include "CommonPCH.h"

#include "Common/FileSystem/WriteOverlayDir.h"

WriteOverlayDir::WriteOverlayDir(const int mask, const std::filesystem::path& basePath)
   : m_mask(mask)
   , m_basePath(basePath)
{
   //nop
}

WriteOverlayDir::~WriteOverlayDir()
{
   //nop
}

const bool WriteOverlayDir::SaveFileData(const std::filesystem::path& path, const std::vector<uint8_t>& data)
{
   const auto localPath = m_basePath / path;
   std::ofstream stream(localPath.c_str(), std::ios::out | std::ios::binary);
   if (false == stream.is_open())
   {
      return false;
   }
   std::ostream_iterator<uint8_t> outIterator(stream);
   std::copy(data.begin(), data.end(), outIterator);
   stream.close();
   return true;
}


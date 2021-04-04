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

//http://www.cplusplus.com/reference/fstream/ofstream/ofstream/
const bool WriteOverlayDir::SaveFileData(const std::filesystem::path& path, const std::vector<uint8_t>& data, const bool bAppend)
{
   const auto localPath = m_basePath / path;

   std::ios::openmode mode = std::ios::out | std::ios::binary | std::ios::ate;
   if (true == bAppend)
   {
      mode |= std::ios::app;
   } 
   else
   {
      mode |= std::ios::trunc;
   }
   std::ofstream stream(localPath.c_str(), mode);
   if (false == stream.is_open())
   {
      return false;
   }
   std::ostream_iterator<uint8_t> outIterator(stream);
   std::copy(data.begin(), data.end(), outIterator);
   stream.close();
   return true;
}

const bool WriteOverlayDir::DeleteSaveFile(const std::filesystem::path& path)
{
   //std::error_code ec;
   if (true == std::filesystem::remove(path))
   {
      return true;
   }
   return false;
}


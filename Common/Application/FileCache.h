#pragma once

//std::vector<uint8_t>
class FileCache
{
public:
   FileCache();
   ~FileCache();

   std::shared_ptr< std::vector<uint8_t> > RequestFile(const std::wstring& path);
   //just load a file without cache
   static std::vector<uint8_t> RawLoadFile(const std::wstring& path);
   static std::string RawLoadFileString(const std::wstring& path);
   

private:
   std::map< std::wstring, std::shared_ptr< std::vector<uint8_t> > > m_fileCache;

};

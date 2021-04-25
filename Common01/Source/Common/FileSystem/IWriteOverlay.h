#pragma once

class IWriteOverlay
{
public:
   virtual ~IWriteOverlay();

   virtual const int GetMask() const = 0;
   //return true if save worked
   virtual const bool WriteFileSaveData(const std::filesystem::path& path, const std::vector<uint8_t>& data, const bool bAppend) = 0;
   //return true if delete worked
   virtual const bool WriteFileDelete(const std::filesystem::path& path) = 0;

};

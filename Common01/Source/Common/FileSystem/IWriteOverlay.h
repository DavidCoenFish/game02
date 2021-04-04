#pragma once

class IWriteOverlay
{
public:
   virtual ~IWriteOverlay();

   virtual const int GetMask() const = 0;
   virtual void SaveFileData(const std::filesystem::path& path, const std::vector<uint8_t>& data) = 0;
   virtual void SaveFileString(const std::filesystem::path& path, const std::string& data) = 0;

};

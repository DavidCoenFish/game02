#pragma once
#include "Common/FileSystem/IWriteOverlay.h"

class WriteOverlayDir : public IWriteOverlay
{
public:
   static const std::filesystem::path GetTempPath();

   WriteOverlayDir(const int mask, const std::filesystem::path& basePath);
   ~WriteOverlayDir();

private:
   virtual const int GetMask() const override;
   virtual void SaveFileData(const std::filesystem::path& path, const std::vector<uint8_t>& data) override;
   virtual void SaveFileString(const std::filesystem::path& path, const std::string& data) override;

private:
   int m_mask;
   std::filesystem::path m_basePath;

};

#pragma once
#include "Common/FileSystem/IWriteOverlay.h"

class WriteOverlayDir : public IWriteOverlay
{
public:

   WriteOverlayDir(const int mask, const std::filesystem::path& basePath);
   ~WriteOverlayDir();

private:
   virtual const int GetMask() const override { return m_mask; }
   //return true if save worked
   virtual const bool SaveFileData(const std::filesystem::path& path, const std::vector<uint8_t>& data) override;

private:
   int m_mask;
   std::filesystem::path m_basePath;

};

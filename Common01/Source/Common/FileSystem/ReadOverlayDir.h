#pragma once
#include "Common/FileSystem/IReadOverlay.h"

class ReadOverlay : public IReadOverlay
{
public:
   ReadOverlay(const int priority, const std::filesystem::path& basePath);
   ~ReadOverlay();

private:
   virtual const int GetPriority() const override { return m_priority; }
   virtual std::shared_ptr< std::vector<uint8_t> > GetFileData(const std::filesystem::path& path) override;
   virtual std::shared_ptr< std::string > GetFileString(const std::filesystem::path& path) override;

private:
   int m_priority;
   std::filesystem::path m_basePath;

};

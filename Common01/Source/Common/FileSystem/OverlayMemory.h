#pragma once
#include "Common/FileSystem/IReadOverlay.h"
#include "Common/FileSystem/IWriteOverlay.h"

class OverlayMemory : public IReadOverlay, public IWriteOverlay
{
public:
   OverlayMemory(
      const int priority, 
      const int mask, 
      const std::map< std::filesystem::path, std::shared_ptr< std::vector<uint8_t>>>& memoryFiles = std::map< std::filesystem::path, std::shared_ptr< std::vector<uint8_t>>>()
      );
   ~OverlayMemory();

private:
   virtual const int GetPriority() const override { return m_priority; }
   virtual std::shared_ptr< std::vector<uint8_t> > ReadFileLoadData(const std::filesystem::path& path) override;

   virtual const int GetMask() const override { return m_mask; }
   virtual const bool WriteFileSaveData(const std::filesystem::path& path, const std::vector<uint8_t>& data, const bool bAppend) override;
   //return true if delete worked
   virtual const bool WriteFileDelete(const std::filesystem::path& path) override;

private:
   int m_priority;
   int m_mask;
   std::map< std::filesystem::path, std::shared_ptr< std::vector<uint8_t>>> m_memoryFiles;

};

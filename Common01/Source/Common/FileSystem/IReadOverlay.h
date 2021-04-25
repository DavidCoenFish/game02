#pragma once

class IReadOverlay
{
public:
   virtual ~IReadOverlay();

   virtual const int GetPriority() const = 0;
   virtual std::shared_ptr< std::vector<uint8_t> > ReadFileLoadData(const std::filesystem::path& path) = 0;

};

#include "CommonPCH.h"

#include "Common/FileSystem/FoundDynamicFile.h"

const int FoundDynamicFile::GetFilter() const
{
   return 0;
}
void FoundDynamicFile::OnProviderChange(IFileSystemProvider* const)
{
   return;
}


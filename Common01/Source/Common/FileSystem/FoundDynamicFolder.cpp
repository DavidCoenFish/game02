#include "CommonPCH.h"

#include "Common/FileSystem/FoundDynamicFolder.h"

const int FoundDynamicFolder::GetFilter() const
{
   return 0;
}

void FoundDynamicFolder::OnProviderChange(IFileSystemProvider* const)
{
   return;
}

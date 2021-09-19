#pragma once

/*
dynamic folders can be created and detroyed
*/
class IFileSystemProvider;

class FoundDynamicFolder
{
public:
   const int GetFilter() const;
   void OnProviderChange(IFileSystemProvider* const pProvider);

};


#pragma once

class IFileSystemProvider
{
public:
   virtual ~IFileSystemProvider();

   virtual void SetFilter(const int filter) = 0; //for FileSystem only?
   virtual const int GetFilter()const = 0;
   virtual void SetCallbacks(ready, changeStatic, changeDynamic) = 0; //onReady/onChange return a map of all the files/ locations, hash of static files... split dynamic & static
   virtual void AsyncStaticFileLoad() = 0;
   virtual void AsyncDynamicFileLoad() = 0;
   virtual void AsyncDynamicFileSave() = 0;
   virtual void AsyncDynamicFileDelete() = 0;
   virtual void AsyncDynamicFolderMake() = 0; //so, does this update map of all files, it probably should, are we going to end up with static and dynamic maps of files?
   virtual void AsyncDynamicFolderRemove() = 0;


};

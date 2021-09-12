#pragma once

/*
do files have a change id? a hash? what if contents of folder change but not the file. 
what if different file from a differnt overlay is now the priority file (file stamp{ mOverlay*, changeId }?)
overlay provides the file

*/

class IFileSystemOverlay;
class FoundFile;

class FoundLocation
{
public:
   ~FoundLocation();

   const bool Exisits() const;
   const int GetFilter() const;

   //mkdir?
   //rmdir?

   void CollectFiles(
      std::vector<std::shared_ptr<FoundFile>>& foundFileArray
      );
   //void VisitFiles(
   //   const std::function<void()>& callback
   //);
   /*
   do we gather files for all the
   */

private:
   int m_filter;
   std::vector< std::weak_ptr< IFileSystemOverlay > > m_pOverlayStack;

};

#include "UnitTestPCH.h"

#include "Common/FileSystem/FileSystem.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

namespace CommonFileSystem
{
#if 0
   TEST_CLASS(Sanity)
   {
   public: 
      TEST_METHOD(Empty)
      {
         auto pOverlay = std::make_shared< OverlayMemory >(0, 1);
         FileSystem::AddReadOverlay(pOverlay);
         FileSystem::AddWriteOverlay(pOverlay);
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(L"", FileSystem::GetTempDir().c_str());
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(L"", FileSystem::GetModualDir(nullptr).c_str());
         FileSystem::ClearReadOverlay();
         FileSystem::ClearWriteOverlay();
      }

      TEST_METHOD(StringPriority)
      {
         std::vector< uint8_t > file0;
         {
            file0.push_back('q');
         }
         std::vector< uint8_t > file1;
         {
            file1.push_back('a');
            file1.push_back('b');
            file1.push_back('c');
         }
         std::vector< uint8_t > file2;
         {
            file2.push_back(1);
            file2.push_back(0);
            file2.push_back(2);
         }
         const auto filePath = std::filesystem::path("foo/bar");
         {
            std::map< std::filesystem::path, std::shared_ptr< std::vector<uint8_t>>> memoryFiles;
            {
               memoryFiles[filePath] = std::make_shared< std::vector< uint8_t > >(file0);
            }
            auto pOverlay = std::make_shared< OverlayMemory >(0, 1, memoryFiles);
            FileSystem::AddReadOverlay(pOverlay);
            FileSystem::AddWriteOverlay(pOverlay);
         }
         {
            std::map< std::filesystem::path, std::shared_ptr< std::vector<uint8_t>>> memoryFiles;
            {
               memoryFiles[filePath] = std::make_shared< std::vector< uint8_t > >(file1);
            }
            auto pOverlay = std::make_shared< OverlayMemory >(2, 1, memoryFiles);
            FileSystem::AddReadOverlay(pOverlay);
            FileSystem::AddWriteOverlay(pOverlay);
         }
         {
            std::map< std::filesystem::path, std::shared_ptr< std::vector<uint8_t>>> memoryFiles;
            {
               memoryFiles[filePath] = std::make_shared< std::vector< uint8_t > >(file2);
            }
            auto pOverlay = std::make_shared< OverlayMemory >(1, 1, memoryFiles);
            FileSystem::AddReadOverlay(pOverlay);
            FileSystem::AddWriteOverlay(pOverlay);
         }

         {
            auto pFile = FileSystem::ReadFileLoadData(filePath);
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pFile.get() );
         }
         {
            auto fileString = FileSystem::DataToString(FileSystem::ReadFileLoadData(filePath));
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual("abc", fileString.c_str() );
         }
         FileSystem::ClearReadOverlay();
         FileSystem::ClearWriteOverlay();
      }
   };
#endif //0
}

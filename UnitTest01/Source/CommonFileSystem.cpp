#include "UnitTestPCH.h"

#include "Common/FileSystem/FileSystem.h"
#include "Common/FileSystem/ProviderMemory.h"
#include "Common/FileSystem/ProviderDisk.h"
#include "Common/FileSystem/FoundStaticFile.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

namespace Microsoft{ namespace VisualStudio {namespace CppUnitTestFramework
{
	template<> inline std::wstring ToString<std::vector<uint8_t>>                  (const std::vector<uint8_t>& t)                  
   { 
      std::wstring result(L"std::vector<uint8_t> size:");
      result += std::to_wstring(t.size());
      result += L" [";
      bool first = true;
      for (const auto& item : t)
      {
         if (true == first)
         {
            first = false;
         }
         else
         {
            result += L", ";
         }
         result += std::to_wstring(item);
      }
      result += L" ]";
      return result;
   }
}}}

namespace CommonFileSystem
{
   TEST_CLASS(Sanity)
   {
   public: 
      TEST_METHOD(StaticMethods)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(L"", FileSystem::GetTempDir().c_str());
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(L"", FileSystem::GetModualDir(nullptr).c_str());
      }

      TEST_METHOD(BasicMemory)
      {
         ProviderMemory::TStaticFileMap staticMapA = {
               {std::filesystem::path("one.txt"), {std::make_shared<std::vector<uint8_t>>(std::vector<uint8_t>({1,2,3,4,5})), 0}}, 
               {std::filesystem::path("two/three.txt"), {std::make_shared<std::vector<uint8_t>>(std::vector<uint8_t>({2,2,3,4,5})), 1}}, 
               {std::filesystem::path("two/five.txt"), {std::make_shared<std::vector<uint8_t>>(std::vector<uint8_t>({3,2,3,4,5})), 2}}, 
               {std::filesystem::path("two/six/seven.txt"), {std::make_shared<std::vector<uint8_t>>(std::vector<uint8_t>({4,2,3,4,5})), 3}}, 
               {std::filesystem::path("eight.j"), {std::make_shared<std::vector<uint8_t>>(std::vector<uint8_t>({5,2,3,4,5})), 4}}, 
               };
         auto pProviderMemoryA = ProviderMemory::Factory(staticMapA);

         std::atomic_bool bFileLoaded = false;
         {
            auto pFileSystem = FileSystem::Factory({pProviderMemoryA});
            {
               auto pFile = pFileSystem->FindStaticFile("one.txt");
               Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, pFile->GetExist());
               pFile->AsyncLoadBest([&](const std::shared_ptr< std::vector< uint8_t > >& data){
                  auto expected = std::vector<uint8_t>({1,2,3,4,5});
                  Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected, *data);
                  bFileLoaded = true;
               });
            }
         }
         //the dtor of the file system waits for all the jobs to finish
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, bFileLoaded.load());
      }
   };

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

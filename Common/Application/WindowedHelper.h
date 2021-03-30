#pragma once

class iApplication;

const int WindowedHelper(
   _In_ HINSTANCE hInstance, 
   _In_ LPWSTR lpCmdLine,
   _In_ int nCmdShow,
   const std::wstring& className,
   const std::wstring& windowTitle = L"",
   const int defaultWindowWidth = 100,
   const int defaultWindowHeight = 100,
   const bool bFullScreen = false,
   const std::function< std::unique_ptr< iApplication >(const std::vector< std::wstring >& cmdLineArray, HWND hwnd, const int width, const int height) >& applicationFactory = nullptr
   );

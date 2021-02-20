#pragma once

class iApplication
{
public:
   virtual ~iApplication();

   //virtual void Initialize(HWND hwnd, const int width, const int height) = 0;

   virtual void Tick() = 0;
   virtual void OnWindowMoved() = 0;
   virtual void OnSuspending() = 0;
   virtual void OnResuming() = 0;
   virtual void OnWindowSizeChanged(const int width, const int height) = 0;
   virtual void OnActivated() = 0;
   virtual void OnDeactivated() = 0;
};

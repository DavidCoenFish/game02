#pragma once

#include "Common/Application/iApplication.h"

class ApplicationDagCollection;
class DrawSystem;
class FileCache;
class Shader;
class Geometry;

class Application : public iApplication
{
public:
   Application(HWND hwnd, const int width, const int height);
   virtual ~Application();

private:
   //virtual void Initialize(HWND hwnd, const int width, const int height) override;
   virtual void Tick() override;
   virtual void OnWindowMoved() override;
   virtual void OnSuspending() override;
   virtual void OnResuming() override;
   virtual void OnWindowSizeChanged(const int width, const int height) override;
   virtual void OnActivated() override;
   virtual void OnDeactivated() override;

   void Render();

private:
   bool m_bTicked;
   std::chrono::steady_clock::time_point m_timePoint;
   std::unique_ptr< FileCache > m_pFileCache;
   std::unique_ptr< DrawSystem > m_pDrawSystem;

   std::unique_ptr< ApplicationDagCollection > m_pApplicationDagCollection;

   //std::shared_ptr< Geometry > m_pGeometry;
   //std::shared_ptr< Shader > m_pShader;

};

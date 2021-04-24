#pragma once

#include "Common/Application/IApplication.h"
class ApplicationHolder;
class DrawSystem;
class DagCollection;

class ApplicationDisplayList : public IApplication
{
   typedef IApplication BaseType;
public:
   ApplicationDisplayList(const IApplicationParam& applicationParam, const std::filesystem::path& rootPath, const std::string& data);
   virtual ~ApplicationDisplayList();

private:
   virtual void Update() override;
   virtual void OnWindowSizeChanged(const int width, const int height) override;

private:
   std::unique_ptr< DrawSystem > m_pDrawSystem;
   std::shared_ptr< DagCollection > m_pDagCollection;

};
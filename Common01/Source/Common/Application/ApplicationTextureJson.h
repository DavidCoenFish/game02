#pragma once

#include "Common/Application/IApplication.h"
#include "Common/DrawSystem/Geometry/Geometry.h"
class ApplicationHolder;
class DrawSystem;
class Shader;
class ShaderTexture;
class GeometryGeneric;

class ApplicationTextureJson : public IApplication
{
   typedef IApplication BaseType;
public:
   static IApplication* const Factory(const HWND hWnd, const IApplicationParam& applicationParam);

   ApplicationTextureJson(const HWND hWnd, const IApplicationParam& applicationParam);
   virtual ~ApplicationTextureJson();

private:
   virtual void Update() override;
   virtual void OnWindowSizeChanged(const int width, const int height) override;

private:
   std::unique_ptr< DrawSystem > m_pDrawSystem;
   std::shared_ptr< Shader > m_pShader;
   std::shared_ptr< ShaderTexture > m_pTexture;
   std::shared_ptr< GeometryGeneric > m_pGeometry;

};
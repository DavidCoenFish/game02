#pragma once

#include "Common/Application/IApplication.h"
//#include "Common/DrawSystem/Geometry/Geometry.h"
class ApplicationHolder;
class DrawSystem;
class Shader;
class GeometryGeneric;

class ApplicationTestTriangle : public IApplication
{
   typedef IApplication BaseType;
public:
   ApplicationTestTriangle(const IApplicationParam& applicationParam, const std::filesystem::path& rootPath);
   virtual ~ApplicationTestTriangle();

private:
   virtual void Update() override;
   virtual void OnWindowSizeChanged(const int width, const int height) override;

private:
   std::unique_ptr< DrawSystem > m_pDrawSystem;

   std::shared_ptr< Shader > m_pShader;
   //struct Vertex
   //{
   //   DirectX::XMFLOAT2 m_position;
   //   DirectX::XMFLOAT4 m_color;
   //};
   //std::shared_ptr< Geometry< Vertex > > m_pGeometry;
   std::shared_ptr< GeometryGeneric > m_pGeometry;


};
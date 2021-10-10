#pragma once

#include "Common/Application/IApplication.h"
class DrawSystem;
class Shader;
template<typename VERTEX_TYPE>
class Geometry;
class RenderTargetTexture;

class ApplicationComputeShader : public IApplication
{
   typedef IApplication BaseType;
public:
   static IApplication* const Factory(const HWND hWnd, const IApplicationParam& applicationParam);

   ApplicationComputeShader(const HWND hWnd, const IApplicationParam& applicationParam);
   virtual ~ApplicationComputeShader();

private:
   virtual void Update() override;
   virtual void OnWindowSizeChanged(const int width, const int height) override;

private:
   std::unique_ptr< DrawSystem > m_pDrawSystem;

   bool m_timePointValid;
   std::chrono::system_clock::time_point m_timePoint;
   float m_timeAccumulate;

   std::shared_ptr< Shader > m_pShaderTriangleMove;
   struct VertexTriangleMove
   {
      DirectX::XMFLOAT2 m_position;
      DirectX::XMFLOAT4 m_color;
   };
   std::shared_ptr< Geometry< VertexTriangleMove > > m_pGeometryTriangleMove;

   //std::shared_ptr< RenderTargetTexture > m_pRenderTargetTexture;

   //int m_computeDataIndex;
   // Desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
   //std::shared_ptr< ShaderTexture > m_pComputeData[3];
   //std::shared_ptr< Shader > m_pShaderCompute;

   //SetCompute32BitConstants
   //Dispatch( Math::DivideByMultiple(dstWidth, 8), Math::DivideByMultiple(dstHeight
   // 
   //std::shared_ptr< Shader > m_pShaderParticle;
   //struct VertexParticle
   //{
   //   DirectX::XMFLOAT2 m_uv;
   //};
   //std::shared_ptr< Geometry< VertexParticle > > m_pGeometryParticle;

};
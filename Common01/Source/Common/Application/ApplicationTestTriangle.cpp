#include "CommonPCH.h"
#include "Common/Application/ApplicationTestTriangle.h"
#include "Common/Log/Log.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/FileSystem/FileSystem.h"
#include "Common/DrawSystem/CustomCommandList.h"
#include "Common/DrawSystem/Shader/ShaderPipelineStateData.h"
#include "Common/DrawSystem/Geometry/GeometryGeneric.h"
#include "Common/DrawSystem/Shader/Shader.h"

ApplicationTestTriangle::ApplicationTestTriangle(const IApplicationParam& applicationParam, const std::filesystem::path& rootPath)
   : IApplication(applicationParam)
{
   LOG_MESSAGE("ApplicationTestTriangle ctor %p", this);
   m_pDrawSystem = std::make_unique< DrawSystem>(applicationParam.m_hWnd);

   std::vector< D3D12_INPUT_ELEMENT_DESC > inputElementDescArray;
   inputElementDescArray.push_back(D3D12_INPUT_ELEMENT_DESC{
      "POSITION",
      0,
      DXGI_FORMAT_R32G32_FLOAT,
      0,
      D3D12_APPEND_ALIGNED_ELEMENT,
      D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
      0 // UINT InstanceDataStepRate;
      });
   inputElementDescArray.push_back(D3D12_INPUT_ELEMENT_DESC{
      "COLOR",
      0,
      DXGI_FORMAT_R32G32B32A32_FLOAT,
      0,
      D3D12_APPEND_ALIGNED_ELEMENT,
      D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
      0 // UINT InstanceDataStepRate;
      });

   auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
   {
      auto pVertexShaderData = FileSystem::GetFileData(rootPath / "VertexShader.cso");
      auto pPixelShaderData = FileSystem::GetFileData(rootPath / "PixelShader.cso");
      std::vector<DXGI_FORMAT> renderTargetFormat;
      renderTargetFormat.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);

      ShaderPipelineStateData shaderPipelineStateData(
         inputElementDescArray,
         D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
         DXGI_FORMAT_UNKNOWN, // DXGI_FORMAT_D32_FLOAT,
         renderTargetFormat,
         CD3DX12_BLEND_DESC(D3D12_DEFAULT),
         CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
         CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
         );

      m_pShader = m_pDrawSystem->MakeShader(
         pCommandList->GetCommandList(),
         shaderPipelineStateData,
         pVertexShaderData,
         nullptr,
         pPixelShaderData
         );
   }

   {
      static const float sData[] = {
         0.0, 0.5,  1.0, 0.0, 0.0, 1.0, 
         0.5, -0.5,   0.0, 1.0, 0.0, 1.0, 
         -0.5, -0.5,  0.0, 0.0, 1.0, 1.0,
      };

      std::vector< float > vertexDataRaw;
      for (int index = 0; index < sizeof(sData) / sizeof(sData[0]); ++index)
      {
         vertexDataRaw.push_back(sData[index]);
      }

      m_pGeometry = m_pDrawSystem->MakeGeometryGeneric(
         pCommandList->GetCommandList(),
         D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
         inputElementDescArray,
         vertexDataRaw,
         6
         );
   }

}

ApplicationTestTriangle::~ApplicationTestTriangle()
{
   if (m_pDrawSystem)
   {
      m_pDrawSystem->WaitForGpu();
   }
   m_pShader.reset();
   m_pGeometry.reset();
   m_pDrawSystem.reset();

   LOG_MESSAGE("ApplicationTestTriangle dtor %p", this);
}

void ApplicationTestTriangle::Update()
{
   BaseType::Update();
   if (m_pDrawSystem)
   {
      auto pFrame = m_pDrawSystem->CreateNewFrame();
      pFrame->SetRenderTarget(m_pDrawSystem->GetRenderTargetBackBuffer());
      pFrame->SetShader(m_pShader.get());
      pFrame->Draw(m_pGeometry.get());
      //m_pDrawSystem->Clear();
      //m_pShader->SetActivate(pFrame->GetCommandList(), pFrame->GetBackBufferIndex());
      //m_pGeometry->Draw(pFrame->GetCommandList());
   }
}

void ApplicationTestTriangle::OnWindowSizeChanged(const int width, const int height)
{
   BaseType::OnWindowSizeChanged(width, height);
   if (m_pDrawSystem)
   {
      m_pDrawSystem->OnResize();
   }

   return;
}

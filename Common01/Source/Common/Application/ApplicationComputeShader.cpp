#include "CommonPCH.h"
#include "Common/Application/ApplicationComputeShader.h"
#include "Common/Log/Log.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/DrawSystem/CustomCommandList.h"
#include "Common/DrawSystem/Shader/Shader.h"
#include "Common/DrawSystem/Shader/ShaderPipelineStateData.h"
#include "Common/DrawSystem/Shader/ShaderConstantInfo.h"
#include "Common/FileSystem/FileSystem.h"

IApplication* const ApplicationComputeShader::Factory(const HWND hWnd, const IApplicationParam& applicationParam)
{
   return new ApplicationComputeShader(hWnd, applicationParam);
}

ApplicationComputeShader::ApplicationComputeShader(const HWND hWnd, const IApplicationParam& applicationParam)
   : IApplication(hWnd, applicationParam)
   , m_timePointValid(false)
   , m_timePoint()
   , m_timeAccumulate(0.0f)
{
   LOG_MESSAGE("ApplicationComputeShader  ctor %p", this);
   m_pDrawSystem = std::make_unique< DrawSystem>(
      hWnd, 
      2, //const unsigned int backBufferCount = 2,
      D3D_FEATURE_LEVEL_11_0, //const D3D_FEATURE_LEVEL d3dFeatureLevel = D3D_FEATURE_LEVEL_11_0,
      0, //const unsigned int options = 0,
      RenderTargetFormatData(DXGI_FORMAT_B8G8R8A8_UNORM, true, VectorFloat4(0.0f,0.0f,0.0f,1.0f)), //const RenderTargetFormatData& targetFormatData = RenderTargetFormatData(DXGI_FORMAT_B8G8R8A8_UNORM),
      RenderTargetDepthData() //const RenderTargetDepthData& targetDepthData = RenderTargetDepthData()
      );

   //make a moving triangle so we have something fun to 
   {
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
         auto pVertexShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / "VertexShaderTriangleMove.cso");
         auto pPixelShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / "PixelShaderTriangleMove.cso");
         std::vector<DXGI_FORMAT> renderTargetFormat;
         renderTargetFormat.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);

         ShaderPipelineStateData shaderPipelineStateData(
            inputElementDescArray,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            DXGI_FORMAT_UNKNOWN, // DXGI_FORMAT_D32_FLOAT,
            renderTargetFormat,
            CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            CD3DX12_DEPTH_STENCIL_DESC() //CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
            );

         std::vector< std::shared_ptr< ShaderConstantInfo > > arrayShaderConstantsInfo;
         arrayShaderConstantsInfo.push_back(std::make_shared<ShaderConstantInfo>(std::vector< uint8_t >({0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}), D3D12_SHADER_VISIBILITY_VERTEX));
         m_pShaderTriangleMove = m_pDrawSystem->MakeShader(
            pCommandList->GetCommandList(),
            shaderPipelineStateData,
            pVertexShaderData,
            nullptr,
            pPixelShaderData,
            std::vector< std::shared_ptr< ShaderResourceInfo > >(),
            arrayShaderConstantsInfo
            );
      }

      {
         std::vector< VertexTriangleMove > vertexData({
            {{0.0f,0.75f}, {1.0f, 1.0f, 1.0f, 1.0f}}, 
            {{0.5f,-0.5}, {0.0f, 0.0f, 0.0f, 1.0f}},
            {{-0.5f,-0.5}, {0.0f, 0.0f, 0.0f, 1.0f}},
            });
         m_pGeometryTriangleMove = m_pDrawSystem->MakeGeometry(
            pCommandList->GetCommandList(),
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
            inputElementDescArray,
            vertexData
            );
      }
   }

}

ApplicationComputeShader ::~ApplicationComputeShader ()
{
   if (m_pDrawSystem)
   {
      m_pDrawSystem->WaitForGpu();
   }
   m_pShaderTriangleMove.reset();
   m_pGeometryTriangleMove.reset();

   m_pDrawSystem.reset();

   LOG_MESSAGE("ApplicationComputeShader dtor %p", this);
}

struct CBuffer
{
   float time;
   float _pad0;
   float _pad1;
   float _pad2;
};

void ApplicationComputeShader ::Update()
{
   BaseType::Update();

   const auto timePointNow = std::chrono::system_clock::now();
   float timeDeltaSeconds = 0.0f;
   if (true == m_timePointValid)
   {
      timeDeltaSeconds = (1.0f / 1000.0f) * (float)(std::chrono::duration_cast<std::chrono::milliseconds>(timePointNow - m_timePoint).count());
   } 
   else
   {
      m_timePointValid = true;
   }
   m_timePoint = timePointNow;
   m_timeAccumulate += timeDeltaSeconds;

   if (m_pDrawSystem)
   {
      m_pShaderTriangleMove->GetConstant<CBuffer>(0).time = m_timeAccumulate * 0.5f;

      auto pFrame = m_pDrawSystem->CreateNewFrame();
      pFrame->SetRenderTarget(m_pDrawSystem->GetRenderTargetBackBuffer());
      pFrame->SetShader(m_pShaderTriangleMove.get());
      pFrame->Draw(m_pGeometryTriangleMove.get());
   }
}

void ApplicationComputeShader ::OnWindowSizeChanged(const int width, const int height)
{
   BaseType::OnWindowSizeChanged(width, height);
   if (m_pDrawSystem)
   {
      m_pDrawSystem->OnResize();
   }

   return;
}

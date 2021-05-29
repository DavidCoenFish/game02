#pragma once

#include "Common/DrawSystem/RenderTarget/RenderTargetFormatData.h"
#include "Common/DrawSystem/RenderTarget/RenderTargetDepthData.h"

class DeviceResources;
class DrawSystemFrame;
class IResource;
class IRenderTarget; //* GetRenderTargetBackBuffer();
class HeapWrapper;
class HeapWrapperItem;
class CustomCommandList;
class Shader;
struct ShaderPipelineStateData;
class ShaderConstantInfo;
class ShaderResourceInfo;
class ShaderConstantBuffer;
class GeometryGeneric;
class ShaderTexture;
namespace DirectX
{
   class GraphicsMemory;
   class GraphicsResource;
};

class DrawSystem
{
public:
   DrawSystem(
      const HWND hWnd,
      const unsigned int backBufferCount = 2,
      const D3D_FEATURE_LEVEL d3dFeatureLevel = D3D_FEATURE_LEVEL_11_0,
      const unsigned int options = 0,
      const RenderTargetFormatData& targetFormatData = RenderTargetFormatData(DXGI_FORMAT_B8G8R8A8_UNORM),
      const RenderTargetDepthData& targetDepthData = RenderTargetDepthData()
      );
   ~DrawSystem();
   void WaitForGpu() noexcept;
   void OnResize();

   void AddResource(IResource* const pResource);
   void RemoveResource(IResource* const pResource);
   const int GetBackBufferIndex() const;
   const int GetBackBufferCount() const { return m_backBufferCount; }

   DirectX::GraphicsResource AllocateConstant(const std::size_t size, void* const pConstants);
   DirectX::GraphicsResource AllocateUpload(
      const std::size_t size, 
      void* const pDataOrNullptr = nullptr, 
      size_t alignment = 16
      );

   std::shared_ptr< Shader > MakeShader(
      ID3D12GraphicsCommandList* const pCommandList,
      const ShaderPipelineStateData& pipelineStateData,
      const std::shared_ptr< std::vector<uint8_t> >& pVertexShaderData,
      const std::shared_ptr< std::vector<uint8_t> >& pGeometryShaderData,
      const std::shared_ptr< std::vector<uint8_t> >& pPixelShaderData,
      const std::vector< std::shared_ptr< ShaderResourceInfo > >& arrayShaderResourceInfo = std::vector< std::shared_ptr< ShaderResourceInfo > >(),
      const std::vector< std::shared_ptr< ShaderConstantInfo > >& arrayShaderConstantsInfo = std::vector< std::shared_ptr< ShaderConstantInfo > >()
      );

   std::shared_ptr< GeometryGeneric > MakeGeometryGeneric(
      ID3D12GraphicsCommandList* const pCommandList,
      const D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
      const std::vector< D3D12_INPUT_ELEMENT_DESC >& inputElementDescArray,
      const std::vector< float >& vertexDataRaw,
      const int floatPerVertex
      );

   std::shared_ptr< ShaderTexture > MakeTexture(
      ID3D12GraphicsCommandList* const pCommandList,
      const std::shared_ptr< HeapWrapperItem >& shaderResource,
      const D3D12_RESOURCE_DESC& desc, 
      const D3D12_SHADER_RESOURCE_VIEW_DESC& shaderResourceViewDesc,
      const std::vector<uint8_t>& data
      );


   std::shared_ptr<CustomCommandList> CreateCustomCommandList();
   void CustomCommandListFinish(ID3D12GraphicsCommandList* pCommandList);

   std::unique_ptr< DrawSystemFrame > CreateNewFrame(); //ctor == Prepare, dtor == Present

   void Prepare(ID3D12GraphicsCommandList*& pCommandList);
   //void Clear();
   void Present();
   IRenderTarget* GetRenderTargetBackBuffer();

   std::shared_ptr<HeapWrapperItem> MakeHeapWrapperCbvSrvUav(const int length = 1);
   std::shared_ptr<HeapWrapperItem> MakeHeapWrapperSampler(const int length = 1);
   std::shared_ptr<HeapWrapperItem> MakeHeapWrapperRenderTargetView(const int length = 1);
   std::shared_ptr<HeapWrapperItem> MakeHeapWrapperDepthStencilView(const int length = 1);

private:
   void CreateDeviceResources();

private:
   HWND m_hWnd;
   unsigned int m_backBufferCount;
   D3D_FEATURE_LEVEL m_d3dFeatureLevel;
   unsigned int m_options;
   RenderTargetFormatData m_targetFormatData;
   RenderTargetDepthData m_targetDepthData;

   std::unique_ptr< DeviceResources > m_pDeviceResources;

   std::list< IResource* > m_listResource;

   std::shared_ptr< HeapWrapper > m_pHeapWrapperCbvSrvUav;
   std::shared_ptr< HeapWrapper > m_pHeapWrapperSampler;
   std::shared_ptr< HeapWrapper > m_pHeapWrapperRenderTargetView;
   std::shared_ptr< HeapWrapper > m_pHeapWrapperDepthStencilView;

};

#pragma once

class iRenderTarget;
class Geometry;
class Shader;
class ShaderTexture2D;

class CustomCommandList
{
public:
   CustomCommandList(
      ID3D12Device* const pDevice,
      ID3D12CommandQueue* const pCommandQueue,
      ID3D12GraphicsCommandList* const pCommandList,
      iRenderTarget* const pRenderTargetOrNull,
      const int fameIndex
      );
   ~CustomCommandList();

   void SetRenderTarget(iRenderTarget* const pRenderTargetOrNull);

   void Draw(
      Geometry* const pGeometry, 
      Shader* const pShader
      );

   void DrawShader(
      Shader* const pShader
      );
   void DrawGeometry(
      Geometry* const pGeometry
      );

   void UpdateSubresourcesMethod(
      ID3D12Resource* pDestinationResource,
      ID3D12Resource* pIntermediate,
      UINT64 IntermediateOffset,
      UINT FirstSubresource,
      UINT NumSubresources,
      D3D12_SUBRESOURCE_DATA* pSrcData,
      D3D12_BOX* pDestBoxOrNull = nullptr
      );
   void ResourceBarrier(
      ID3D12Resource* const pResource,
      D3D12_RESOURCE_STATES stateBefore,
      D3D12_RESOURCE_STATES stateAfter
      );

private:
   ID3D12Device* m_pDevice;
   ID3D12CommandQueue* m_pCommandQueue;
   ID3D12GraphicsCommandList* m_pCommandList;
   iRenderTarget* m_pRenderTarget;
   int m_frameIndex;

};

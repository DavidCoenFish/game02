#pragma once

class HeapWrapperItem;

class ShaderResourceInfo
{
public:
   static std::shared_ptr< ShaderResourceInfo > FactorySampler(
      const std::shared_ptr< HeapWrapperItem >& shaderResourceViewHandle,
      const D3D12_SHADER_VISIBILITY visiblity, //D3D12_SHADER_VISIBILITY_PIXEL
      const bool bMipMap = false
      );
   static std::shared_ptr< ShaderResourceInfo > FactoryDataSampler(
      const std::shared_ptr< HeapWrapperItem >& shaderResourceViewHandle,
      const D3D12_SHADER_VISIBILITY visiblity
      );
   static std::shared_ptr< ShaderResourceInfo > FactoryNoSampler(
      const std::shared_ptr< HeapWrapperItem >& shaderResourceViewHandle,
      const D3D12_SHADER_VISIBILITY visiblity
      );

   explicit ShaderResourceInfo(
      const std::shared_ptr< HeapWrapperItem >& shaderResourceViewHandle = nullptr,
      const D3D12_STATIC_SAMPLER_DESC& staticSamplerDesc = CD3DX12_STATIC_SAMPLER_DESC(),
      const bool bUseSampler = true
      );

   void Activate( 
      ID3D12GraphicsCommandList* const pCommandList,
      const int rootParamIndex
      );
   const bool GetUseSampler() const { return m_bUseSampler; }
   const D3D12_STATIC_SAMPLER_DESC& GetStaticSamplerDesc() const { return m_staticSamplerDesc; }
   const D3D12_SHADER_VISIBILITY GetVisiblity() const;

private:
   bool m_bUseSampler;
   std::shared_ptr< HeapWrapperItem > m_shaderResourceViewHandle;
   D3D12_STATIC_SAMPLER_DESC m_staticSamplerDesc;

};

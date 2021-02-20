#pragma once

class ShaderConstantInfo
{
public:
   explicit ShaderConstantInfo(
      const std::vector< uint8_t >& data = std::vector< uint8_t >(),
      const D3D12_SHADER_VISIBILITY visiblity  = D3D12_SHADER_VISIBILITY_ALL
      );
   void UpdateData(void* const pData, const size_t dataSize);

   const D3D12_SHADER_VISIBILITY GetVisiblity() const { return m_visiblity; }
   const size_t GetBufferSize() const { return m_data.size(); }
   const void* const GetBufferData() const { return m_data.data(); }

private:
   const D3D12_SHADER_VISIBILITY m_visiblity;
   std::vector< uint8_t > m_data;

};

#include "CommonPCH.h"

#include "Common/DrawSystem/Shader/ShaderConstantInfo.h"

ShaderConstantInfo::ShaderConstantInfo(
   const std::vector< uint8_t >& data,
   const D3D12_SHADER_VISIBILITY visiblity
   )
   : m_visiblity(visiblity)
   , m_data(data)
{
}

void ShaderConstantInfo::UpdateData(const void* const pData, const size_t dataSize)
{
   dataSize;
   assert(dataSize == m_data.size());
   memcpy(m_data.data(), pData, m_data.size());
   return;
}

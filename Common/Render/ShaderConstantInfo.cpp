#include "CommonPCH.h"

#include "Common/Render/ShaderConstantInfo.h"


ShaderConstantInfo::ShaderConstantInfo(
   const std::vector< uint8_t >& data,
   const D3D12_SHADER_VISIBILITY visiblity
   )
   : m_visiblity(visiblity)
   , m_data(data)
{
}

void ShaderConstantInfo::UpdateData(void* const pData, const size_t dataSize)
{
   assert(dataSize == m_data.size());
   memcpy(m_data.data(), pData, m_data.size());
   return;
}

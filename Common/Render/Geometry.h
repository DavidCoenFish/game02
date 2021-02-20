#pragma once

#include "Common/Render/iResource.h"

class Geometry : public iResource
{
public:
   Geometry(
      DrawSystem* const pDrawSystem,
      const D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
      const std::vector< D3D12_INPUT_ELEMENT_DESC >& inputElementDescArray,
      const int floatPerVertex,
      const std::vector< float >& vertexRawData
      );
   virtual ~Geometry();

   void Draw(ID3D12GraphicsCommandList* const pCommandList);

private:
   virtual void OnDeviceLost() override;
   virtual void OnDeviceRestored(
      CustomCommandList* const pCommandList,
      ID3D12Device* const pDevice
      ) override;

private:
   const D3D_PRIMITIVE_TOPOLOGY m_primitiveTopology;
   const std::vector< D3D12_INPUT_ELEMENT_DESC > m_inputElementDescArray;
   const int m_floatPerVertex;
   std::vector< float > m_vertexRawData;

   Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
   D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

};

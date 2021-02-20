#include "CommonPCH.h"
#include "Common/JSON/JSONGeometry.h"
#include "Common/JSON/JSONInputElementDesc.h"
#include "Common/JSON/JSONEnum.h"
#include "Common/Render/CustomCommandList.h"
#include "Common/Render/DrawSystem.h"
#include "Common/Render/Geometry.h"

NLOHMANN_JSON_SERIALIZE_ENUM( D3D_PRIMITIVE_TOPOLOGY, {
   {D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, nullptr},
   {TOKEN_PAIR(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED)},
   {TOKEN_PAIR(D3D_PRIMITIVE_TOPOLOGY_POINTLIST)},
   {TOKEN_PAIR(D3D_PRIMITIVE_TOPOLOGY_LINELIST)},
   {TOKEN_PAIR(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP)},
   {TOKEN_PAIR(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)},
   {TOKEN_PAIR(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)},
   {TOKEN_PAIR(D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ)},
   {TOKEN_PAIR(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ)},
   {TOKEN_PAIR(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ)},
   {TOKEN_PAIR(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ)}
});

struct JSONGeometryData
{
   D3D_PRIMITIVE_TOPOLOGY topology;
   int floatPerVertex;
   std::vector< float > vertexData;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONGeometryData, 
   topology, 
   floatPerVertex, 
   vertexData
   );

std::shared_ptr< Geometry > JSONGeometry::Factory(
   DrawSystem* const pDrawSystem,
   CustomCommandList* const pCommandList,
   const nlohmann::json& json
   )
{
   auto geometryData = json.get< JSONGeometryData >();
   std::vector< D3D12_INPUT_ELEMENT_DESC > inputElementDesc = JSONInputElementDesc::Factory(json);
   auto pResult = pDrawSystem->GeometryFactory(
      pCommandList,
      geometryData.topology,
      inputElementDesc,
      geometryData.floatPerVertex,
      geometryData.vertexData
      );
   return pResult;
}

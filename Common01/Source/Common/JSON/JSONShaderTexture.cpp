#include "CommonPCH.h"
#include "Common/JSON/JSONShaderTexture.h"
#include "Common/JSON/JSONDrawSystemEnum.h"
#include "Common/JSON/JSONInputElementDesc.h"

//typedef struct D3D12_RESOURCE_DESC
//    {
//    D3D12_RESOURCE_DIMENSION Dimension;
//    UINT64 Alignment;
//    UINT64 Width;
//    UINT Height;
//    UINT16 DepthOrArraySize;
//    UINT16 MipLevels;
//    DXGI_FORMAT Format;
//    DXGI_SAMPLE_DESC SampleDesc;
//    D3D12_TEXTURE_LAYOUT Layout;
//    D3D12_RESOURCE_FLAGS Flags;
//    } 	D3D12_RESOURCE_DESC;
//void from_json(const nlohmann::json& j, D3D12_RESOURCE_DESC& p)
//{
//}


//typedef struct D3D12_TEX2D_SRV
//    {
//    UINT MostDetailedMip;
//    UINT MipLevels;
//    UINT PlaneSlice;
//    FLOAT ResourceMinLODClamp;
//    } 	
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   D3D12_TEX2D_SRV,
   MostDetailedMip,
   MipLevels,
   PlaneSlice,
   ResourceMinLODClamp
);


//typedef struct D3D12_SHADER_RESOURCE_VIEW_DESC {
//  DXGI_FORMAT         Format;
//  D3D12_SRV_DIMENSION ViewDimension;
//  UINT                Shader4ComponentMapping;
//  union {
//    D3D12_BUFFER_SRV                            Buffer;
//    D3D12_TEX1D_SRV                             Texture1D;
//    D3D12_TEX1D_ARRAY_SRV                       Texture1DArray;
//    D3D12_TEX2D_SRV                             Texture2D;
//    D3D12_TEX2D_ARRAY_SRV                       Texture2DArray;
//    D3D12_TEX2DMS_SRV                           Texture2DMS;
//    D3D12_TEX2DMS_ARRAY_SRV                     Texture2DMSArray;
//    D3D12_TEX3D_SRV                             Texture3D;
//    D3D12_TEXCUBE_SRV                           TextureCube;
//    D3D12_TEXCUBE_ARRAY_SRV                     TextureCubeArray;
//    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_SRV RaytracingAccelerationStructure;
//  };
//} D3D12_SHADER_RESOURCE_VIEW_DESC;
void to_json(nlohmann::json& j, const D3D12_SHADER_RESOURCE_VIEW_DESC& p)
{
   j; p;
}
void from_json(const nlohmann::json& j, D3D12_SHADER_RESOURCE_VIEW_DESC& p)
{
   memset(&p, 0, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
   if(j.contains("Format"))
   { 
      from_json(j.at("Format"), p.Format); 
   }
   if(j.contains("ViewDimension"))
   { 
      from_json(j.at("ViewDimension"), p.ViewDimension); 
   }
   if(j.contains("Shader4ComponentMapping"))
   {
      if (j.at("Shader4ComponentMapping").is_string())
      {
         std::string value;
         //from_json(j.at("Shader4ComponentMapping"), value); 
         j.at("Shader4ComponentMapping").get_to(value);
         if (value == "D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING")
         {
            p.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
         }
         //other defines?
      }
      else
      {
         //other encode
      }
   }
   if (j.contains("Texture2D"))
   {
      from_json(j.at("Texture2D"), p.Texture2D); 
   }
}

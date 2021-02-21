#include "BarrelDistortion.hlsli"

struct Interpolants
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
};

struct Pixel
{
    float4 color    : SV_TARGET0;
};

cbuffer CameraConstants : register(b0)
{
    float3 m_cameraPosition;
    float3 m_cameraDirection;
    float3 m_cameraUp;
    float4 m_cameraFovhFovvDegreesNearFar;
};

cbuffer PixelConstants : register(b1)
{
    float4 m_posRadius;
};

//https://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection/
float HitSphere(float3 spherePos, float sphereRadius, float3 rayPos, float3 rayDir)
{
   float3 oc = rayPos - spherePos;
   float a = dot(rayDir, rayDir);
   float b = 2.0 * dot(oc, rayDir);
   float c = dot(oc,oc) - sphereRadius*sphereRadius;
   float discriminant = b*b - 4*a*c;
   if(discriminant < 0.0)
   {
      //discard;
      return -1.0;
   }
   else
   {
      float sqrtDiscriminant = sqrt(discriminant);
      float numerator = -b - sqrtDiscriminant;
      if (0.0f < numerator)
      {
         return numerator / (2.0 * a);
      }

      numerator = -b + sqrtDiscriminant;
      if (0.0f < numerator)
      {
         return numerator / (2.0 * a);
      }
   }
   //discard;
   return -1.0;
}

Pixel main( Interpolants In )
{
   float3 worldRay = WorldRayFromUV(In.uv, m_cameraFovhFovvDegreesNearFar.xy, m_cameraUp, m_cameraDirection);
   float t = HitSphere(m_posRadius.xyz, m_posRadius.w, m_cameraPosition, worldRay);
   if (t < 0.0)
   {
      discard;
   }

   Pixel Out;
   Out.color = float4(t, 1.0, 1.0, 1.0);
   return Out;
}
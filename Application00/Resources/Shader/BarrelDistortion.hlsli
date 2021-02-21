float2 MakeScreenRadians(float2 uv, float2 fovhFovv)
{
   float2 result = (uv - 0.5f) * radians(fovhFovv);
   return result;
}

float3 PolarToVector(float2 polarCoordRadian) //radians
{
   float baseX = 0.0f; //sin(polarCoordRadian.x);
   float baseZ = 0.0f; //cos(polarCoordRadian.x);
   sincos(polarCoordRadian.x, baseX, baseZ);
   float riseBase = 0.0f; //cos(polarCoordRadian.y);
   float y = 0.0f; //sin(polarCoordRadian.y);
   sincos(polarCoordRadian.y, y, riseBase);
   float x = baseX * riseBase;
   float z = baseZ * riseBase;
   return float3(x, y, z);
}

float3 MakeWorldRay(float3 screenEyeRay, float3 cameraRight, float3 cameraUp, float3 cameraAt)
{
   float3 worldRay = (screenEyeRay.x * cameraRight) + (screenEyeRay.y * cameraUp) + (screenEyeRay.z * cameraAt);
   return worldRay;
}

float3 WorldRayFromUV(float2 uv, float2 fovhFovv, float3 cameraUp, float3 cameraAt)
{
   float2 screenRadians = MakeScreenRadians(uv, fovhFovv);
   float3 screenEyeNormal = PolarToVector(screenRadians);
   float3 cameraRight = cross(cameraUp, cameraAt);
   float3 worldRay = MakeWorldRay(screenEyeNormal, cameraRight, cameraUp, cameraAt);
   return worldRay;
}

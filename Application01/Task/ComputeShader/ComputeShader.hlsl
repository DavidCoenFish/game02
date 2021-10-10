//https://www.3dgep.com/learning-directx-12-4/

#define BLOCK_SIZE 8

cbuffer ComputeShaderConstantBuffer : register( b0 )
{
    float2 texelSize; // 1.0 / txOutput0.Dimensions
    float timeDelta;
    float timeAccumulate;
};

struct ComputeShaderInput
{
    uint3 GroupID           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 GroupThreadID     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 DispatchThreadID  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  GroupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

Texture2D<float4> txInput : register( t0 );
SamplerState samLinear : register(s0);

RWTexture2D<float4> txData0 : register( u0 ); //output particle state, x,y,z,lifespan
RWTexture2D<float4> txData1 : register( u1 ); //previous state 0
RWTexture2D<float4> txData2 : register( u2 ); //previous state 1

// Gold Noise (c)2015 dcerisano@standard3d.com
// - based on the Golden Ratio
// - uniform normalized distribution
// - fastest static noise generator function (also runs at low precision)
// - use with indicated seeding method. 
float PHI = 1.61803398874989484820459;  // phi = Golden Ratio   
float gold_noise(float2 xy, float seed){
   return frac(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

[numthreads( BLOCK_SIZE, BLOCK_SIZE, 1 )]
void main( ComputeShaderInput IN )
{
    float2 uv = texelSize * ( IN.DispatchThreadID.xy + 0.5 );
    float4 source = txInput.SampleLevel( samLinear, uv, 0 );
    float sourceAverage = (source.r + source.g + source.b) / 3.0;
    float rand = gold_noise(uv, frac(timeAccumulate));
    float4 prevData0 = txData1[IN.DispatchThreadID.xy];
    float4 prevData1 = txData2[IN.DispatchThreadID.xy];
    float4 newData = (float4)0;

    if ((rand < sourceAverage) && (prevData0.w <= 0.0))
    {
       //emit new
       newData = float4(uv.x, uv.y, 0.0, 1.0);
    }
    else if(0.0 < prevData0.w) //still alive
    {
       newData.w = prevData0.w - (timeDelta * 0.25);

       if (prevData0.w < prevData1.w) //we have continuity with last frame
       {
          newData.x = prevData0.x + (prevData0.x - prevData1.x);
          newData.y = prevData0.y + (prevData0.y - prevData1.y) + (timeDelta * 0.1);
          newData.z = prevData0.z + (prevData0.z - prevData1.z);
       }
       else //no continuity, make up values for velocity
       {
          float rand1 = gold_noise(uv, rand);
          float rand2 = gold_noise(uv, rand1);

          newData.x = (rand1 - 0.5) * 0.1;
          newData.y = (rand2 - 0.5) * 0.1;
          newData.z = rand * timeDelta;
       }
    }

    txData0[IN.DispatchThreadID.xy] = newData;
}

